//  88-Line 2D Moving Least Squares Material Point Method (MLS-MPM)
// [Explained Version by David Medina]

// Uncomment this line for image exporting functionality
#define TC_IMAGE_IO

// Note: You DO NOT have to install taichi or taichi_mpm.
// You only need [taichi.h] - see below for instructions.
#include "taichi.h"

using namespace taichi;
using Vec = Vector2;
using Mat = Matrix2;

// Window
const int window_size = 600;

// Grid resolution (cells)
const int n = 80;

const real dt = 1e-4_f;
const real frame_dt = 1e-3_f;
const real dx = 1.0_f / n;
const real inv_dx = 1.0_f / dx;

// Snow material properties
const auto particle_mass = 1.0_f;
const auto vol = 1.0_f;        // Particle Volume
//const auto hardening = 10.0_f; // Snow hardening factor
const auto hardening = 10.0_f;
//const auto E = 1e4_f;          // Young's Modulus
const auto E = 1e4_f;   // 1e_3f 1e4_f 3e_4f
//const auto nu = 0.2_f;         // Poisson ratio
const auto nu = 0.2_f;         // Poisson ratio
const bool plastic = true;

// Initial Lamé parameters
const real mu_0 = E / (2 * (1 + nu));
const real lambda_0 = E * nu / ((1+nu) * (1 - 2 * nu));

struct Particle {
  // Position and velocity
  Vec x, v;
  // Deformation gradient
  Mat F;
  // Affine momentum from APIC
  Mat C;
  // Determinant of the deformation gradient (i.e. volume)
  real Jp;
  // Color
  int c;
  // 0: fluid 1: jelly 2: snow 
  int ptype;

  Particle(Vec x, int c, Vec v=Vec(0), int ptype=2) :
    x(x),
    v(v),
    F(1),
    C(0),
    Jp(1),
    c(c),
    ptype(ptype){}
};

std::vector<Particle> particles;

// Vector3: [velocity_x, velocity_y, mass]
Vector3 grid[n + 1][n + 1];

void advance(real dt) {
  // Reset grid
  std::memset(grid, 0, sizeof(grid));

  // P2G
  for (auto &p : particles) {
    // element-wise floor
    Vector2i base_coord = (p.x * inv_dx - Vec(0.5f)).cast<int>();

    Vec fx = p.x * inv_dx - base_coord.cast<real>();

    // Quadratic kernels [http://mpm.graphics Eqn. 123, with x=fx, fx-1,fx-2]
    Vec w[3] = {
      Vec(0.5) * sqr(Vec(1.5) - fx),
      Vec(0.75) - sqr(fx - Vec(1.0)),
      Vec(0.5) * sqr(fx - Vec(0.5))
    };

    // Compute current Lamé parameters [http://mpm.graphics Eqn. 86]
    //auto e = std::exp(hardening * (1.0f - p.Jp));
    //auto mu = mu_0 * e;
    //auto lambda = lambda_0 * e;
    auto e = std::exp(hardening * (1.0_f - p.Jp));
    if (p.ptype == 1)e = 0.3;
    auto mu = mu_0 * e, lambda = lambda_0 * e;
    if (p.ptype == 0)mu = 0;


    // Current volume
    real J = determinant(p.F);

    // Polar decomposition for fixed corotated model
    Mat r, s;
    polar_decomp(p.F, r, s);

    // [http://mpm.graphics Paragraph after Eqn. 176]
    real Dinv = 4 * inv_dx * inv_dx;
    // [http://mpm.graphics Eqn. 52]
    auto PF = (2 * mu * (p.F-r) * transposed(p.F) + lambda * (J-1) * J);

    // Cauchy stress times dt and inv_dx
    auto stress = - (dt * vol) * (Dinv * PF);

    // Fused APIC momentum + MLS-MPM stress contribution
    // See http://taichi.graphics/wp-content/uploads/2019/03/mls-mpm-cpic.pdf
    // Eqn 29
    auto affine = stress + particle_mass * p.C;

    // P2G
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        auto dpos = (Vec(i, j) - fx) * dx;
        // Translational momentum
        Vector3 mass_x_velocity(p.v * particle_mass, particle_mass);
        grid[base_coord.x + i][base_coord.y + j] += (
          w[i].x*w[j].y * (mass_x_velocity + Vector3(affine * dpos, 0))
        );
      }
    }
  }

  // For all grid nodes
  for(int i = 0; i <= n; i++) {
    for(int j = 0; j <= n; j++) {
      auto &g = grid[i][j];
      // No need for epsilon here
      if (g[2] > 0) {
        // Normalize by mass
        g /= g[2];
        // Gravity
        g += dt * Vector3(0, -200, 0);

        // boundary thickness
        real boundary = 0.05;
        // Node coordinates
        real x = (real) i / n;
        real y = real(j) / n;

        // Sticky boundary
        if (x < boundary || x > 1-boundary || y > 1-boundary) {
          g = Vector3(0);
        }
        // Separate boundary
        if (y < boundary) {
          g[1] = std::max(0.0f, g[1]);
        }
      }
    }
  }

  // G2P
  for (auto &p : particles) {
    // element-wise floor
    Vector2i base_coord = (p.x * inv_dx - Vec(0.5f)).cast<int>();
    Vec fx = p.x * inv_dx - base_coord.cast<real>();
    Vec w[3] = {
                Vec(0.5) * sqr(Vec(1.5) - fx),
                Vec(0.75) - sqr(fx - Vec(1.0)),
                Vec(0.5) * sqr(fx - Vec(0.5))
    };

    p.C = Mat(0);
    p.v = Vec(0);

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        auto dpos = (Vec(i, j) - fx);
        auto grid_v = Vec(grid[base_coord.x + i][base_coord.y + j]);
        auto weight = w[i].x * w[j].y;
        // Velocity
        p.v += weight * grid_v;
        // APIC C
        p.C += 4 * inv_dx * Mat::outer_product(weight * grid_v, dpos);
      }
    }

    // Advection
    p.x += dt * p.v;

    // MLS-MPM F-update
    auto F = (Mat(1) + dt * p.C) * p.F;

    //  Mat svd_u, sig, svd_v;
    //  svd(F, svd_u, sig, svd_v);

    if (p.ptype == 0) { p.F = Mat(1) * sqrt(determinant(F)); }
    else if (p.ptype == 1) { p.F = F; }
    else if (p.ptype == 2)
    {
        Mat svd_u, sig, svd_v; svd(F, svd_u, sig, svd_v);
        for (int i = 0; i < 2 * int(plastic); i++)
            sig[i][i] = clamp(sig[i][i], 1.0_f - 2.5e-2_f, 1.0_f + 7.5e-3_f);
        real oldJ = determinant(F); F = svd_u * sig * transposed(svd_v);
        real Jp_new = clamp(p.Jp * oldJ / determinant(F), 0.6_f, 20.0_f);
        p.Jp = Jp_new; p.F = F;
    }


    // Snow Plasticity
    //  for (int i = 0; i < 2 * int(plastic); i++) {
      //    sig[i][i] = clamp(sig[i][i], 1.0f - 2.5e-2f, 1.0f + 7.5e-3f);
    //}

    //real oldJ = determinant(F);
    //F = svd_u * sig * transposed(svd_v);

    //real Jp_new = clamp(p.Jp * oldJ / determinant(F), 0.6f, 20.0f);

    //p.Jp = Jp_new;
    //p.F = F;
  }
}

// Seed particles with position and color
void add_object(Vec center, int c, int ptype=2) {
  // Randomly sample 1000 particles in the square
  for (int i = 0; i < 1000; i++) {
    particles.push_back(Particle((Vec::rand()*2.0f-Vec(1))*0.08_f + center, c, Vec(0,0), ptype));
  }
}
/**************************************************************************************/
void add_object_rectangle(Vec v1, Vec v2, int c, int num = 500, Vec velocity = Vec(0.0_f), int ptype = 0)
{
    Vec box_min(min(v1.x, v2.x), min(v1.y, v2.y)), box_max(max(v1.x, v2.x), max(v1.y, v2.y));
    int i = 0;
    while (i < num) {
        auto pos = Vec::rand();
        if (pos.x > box_min.x&& pos.y > box_min.y&& pos.x < box_max.x && pos.y < box_max.y) {
            particles.push_back(Particle(pos, c, velocity, ptype));
            i++;
        }
    }
}

//  jelly horse
void add_object_horse(Vec v, int c, int num=2000, Vec velocity = Vec(10.0, 0), int ptype = 1)
{
    int i = 0;
    while (i < num / 5)
    {
        auto pos = Vec::rand();
        if (pos.x > v.x&& pos.x < v.x + 0.05 && pos.y>0.04 && pos.y < 0.09)
        {
            particles.push_back(Particle(pos, c, Vec(0,0), ptype));
            i++;
        }
    }
    i = 0;
    while (i < num / 5)
    {
        auto pos = Vec::rand();
        if (pos.x > v.x+0.1&& pos.x < v.x + 0.15 && pos.y>0.04 && pos.y < 0.09)
        {
            particles.push_back(Particle(pos, c, velocity, ptype));
            i++;
        }
    }
    i = 0;
    while (i < num*3 / 5)
    {
        auto pos = Vec::rand();
        if (pos.x > v.x && pos.x < v.x + 0.15 && pos.y>0.09 && pos.y < 0.14)
        {
            particles.push_back(Particle(pos, c, Vec(0,0), ptype));
            i++;
        }
    }
}

void add_object_circle(Vec v, real r, int c, int num = 1000, Vec velocity = Vec(0.0_f), int ptype = 0)
{
    
    int i = 0;
    while (i < num) {
        auto pos = Vec::rand();
        if ((pos.x-v.x)*(pos.x-v.x)+(pos.y-v.y)*(pos.y-v.y)<r*r) {
            particles.push_back(Particle(pos, c, velocity, ptype));
            i++;
        }
    }
}

void add_object_triangle(Vec v1, Vec v2, int c, int num = 500, Vec velocity = Vec(0.0_f), int ptype = 2)
{
    Vec box_min(min(v1.x, v2.x), min(v1.y, v2.y)), box_max(max(v1.x, v2.x), max(v1.y, v2.y));
    int i = 0;
    while (i < num) {
        auto pos = Vec::rand();
        if (pos.x > box_min.x&& pos.y > box_min.y&& pos.x < box_max.x && pos.y < box_max.y) {
            real mid_x = (box_min.x + box_max.x) / 2;
            if ((pos.y - box_max.y) * (mid_x - box_min.x) - (box_min.y - box_max.y) * (pos.x - box_min.x) > 0)
            {
                if ((pos.y - box_max.y) * (mid_x - box_max.x) - (box_min.y - box_max.y) * (pos.x - box_max.x) < 0)
                {
                    particles.push_back(Particle(pos, c, velocity, ptype));
                    i++;
                }
            }



        }
    }
}

void add_jet(Vec v1, Vec v2, int c, Vec vel, int ptype) {
    add_object_rectangle(v1, v2, c, 10, vel, ptype);
    //add_object_rectangle(Vec(0.5, 0.5), Vec(0.51, 0.51), 0x87CEFA, 10, Vec(0.0, -10.0));
}

////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  GUI gui("Real-time 2D MLS-MPM", window_size, window_size);
  auto &canvas = gui.get_canvas();

  //add_object_circle(Vec(0.50, 0.90), 0.02, 0x00FF00, 1000, Vec(0.0, -20.0), 1);
  //add_object_triangle(Vec(0.45,0.85), Vec(0.55,0.95), 0x00FF00,1000, Vec(0,-20));
  add_object_rectangle(Vec(0.40, 0.88), Vec(0.60, 0.92), 0x00FF00, 1000, Vec(0.0, -20.0), 1);
  //add_object_horse(Vec(0.20, 0.04), 0xFAFAFA);
  add_object_rectangle(Vec(0.05, 0.04), Vec(0.95, 0.54), 0x1E90FF, 50000, Vec(0.0,0.0), 0);


  int frame = 0;
  // Main Loop
  for (int step = 0;; step++) {
    // Advance simulation
    advance(dt);

    // Visualize frame
    if (step % int(frame_dt / dt) == 0) {
      // Clear background
      canvas.clear(0x112F41);
      // Box
      canvas.rect(Vec(0.04), Vec(0.96)).radius(2).color(0x4FB99F).close();
      // Particles
     
      for (auto p : particles) {
        canvas.circle(p.x).radius(2).color(p.c);
      }
     

      // Write to disk (optional)
      
      /**************************************************************************/
      /*if (step < 2e3)
      {
          add_jet(Vec(0.05, 0.14), Vec(0.06, 0.15), 0x87CEFA, Vec(40.0, 0.0), 0);
      }*/
      // Update image
      gui.update();

      canvas.img.write_as_image(fmt::format("flat_shoot/{:05d}.png", frame++));
      /////////////////////////////////////////////////////////////////////////////////
    }
  }
}

/* -----------------------------------------------------------------------------
** Reference: A Moving Least Squares Material Point Method with Displacement
              Discontinuity and Two-Way Rigid Body Coupling (SIGGRAPH 2018)

  By Yuanming Hu (who also wrote this 88-line version), Yu Fang, Ziheng Ge,
           Ziyin Qu, Yixin Zhu, Andre Pradhana, Chenfanfu Jiang


** Build Instructions:

Step 1: Download and unzip mls-mpm88.zip (Link: http://bit.ly/mls-mpm88)
        Now you should have "mls-mpm88.cpp" and "taichi.h".

Step 2: Compile and run

* Linux:
    g++ mls-mpm88-explained.cpp -std=c++14 -g -lX11 -lpthread -O3 -o mls-mpm
    ./mls-mpm


* Windows (MinGW):
    g++ mls-mpm88-explained.cpp -std=c++14 -lgdi32 -lpthread -O3 -o mls-mpm
    .\mls-mpm.exe


* Windows (Visual Studio 2017+):
  - Create an "Empty Project"
  - Use taichi.h as the only header, and mls-mpm88-explained.cpp as the only source
  - Change configuration to "Release" and "x64"
  - Press F5 to compile and run


* OS X:
    g++ mls-mpm88-explained.cpp -std=c++14 -framework Cocoa -lpthread -O3 -o mls-mpm
    ./mls-mpm


** FAQ:
Q1: What does "1e-4_f" mean?
A1: The same as 1e-4f, a float precision real number.

Q2: What is "real"?
A2: real = float in this file.

Q3: What are the hex numbers like 0xED553B?
A3: They are RGB color values.
    The color scheme is borrowed from
    https://color.adobe.com/Copy-of-Copy-of-Core-color-theme-11449181/

Q4: How can I get higher-quality?
A4: Change n to 320; Change dt to 1e-5; Change E to 2e4;
    Change particle per cube from 500 to 8000 (Ln 72).
    After the change the whole animation takes ~3 minutes on my computer.

Q5: How to record the animation?
A5: Uncomment Ln 2 and 85 and create a folder named "tmp".
    The frames will be saved to "tmp/XXXXX.png".

    To get a video, you can use ffmpeg. If you already have taichi installed,
    you can simply go to the "tmp" folder and execute

      ti video 60

    where 60 stands for 60 FPS. A file named "video.mp4" is what you want.

Q6: How is taichi.h generated?
A6: Please check out my #include <taichi> talk:
    http://taichi.graphics/wp-content/uploads/2018/11/include_taichi.pdf
    and the generation script:
    https://github.com/yuanming-hu/taichi/blob/master/misc/amalgamate.py
    You can regenerate it using `ti amal`, if you have taichi installed.

Questions go to yuanming _at_ mit.edu
                            or https://github.com/yuanming-hu/taichi_mpm/issues.

                                                      Last Update: March 6, 2019
                                                      Version 1.5

----------------------------------------------------------------------------- */
