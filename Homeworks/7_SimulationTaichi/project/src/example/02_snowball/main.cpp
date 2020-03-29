//88-Line 2D Moving Least Squares Material Point Method (MLS-MPM)[with comments]
//#define TC_IMAGE_IO   // Uncomment this line for image exporting functionality
#include <taichi.h>    // Note: You DO NOT have to install taichi or taichi_mpm.
using namespace taichi;// You only need [taichi.h] - see below for instructions.
const int n = 80 /*grid resolution (cells)*/, window_size = 800;
const real dt = 1e-4_f, frame_dt = 1e-3_f, dx = 1.0_f / n, inv_dx = 1.0_f / dx;
auto particle_mass = 1.0_f, vol = 1.0_f;
auto hardening = 10.0_f, E = 1e4_f, nu = 0.2_f;
real mu_0 = E / (2 * (1 + nu)), lambda_0 = E * nu / ((1+nu) * (1 - 2 * nu));
using Vec = Vector2; using Mat = Matrix2; bool plastic = true;
struct Particle { Vec x, v; Mat F, C; real Jp; int c/*color*/;
  Particle(Vec x, int c, Vec v=Vec(0)) : x(x), v(v), F(1), C(0), Jp(1), c(c){}};
std::vector<Particle> particles;
Vector3 grid[n + 1][n + 1];          // velocity + mass, node_res = cell_res + 1

void advance(real dt) {
  std::memset(grid, 0, sizeof(grid));                              // Reset grid
  for (auto &p : particles) {                                             // P2G
    Vector2i base_coord=(p.x*inv_dx-Vec(0.5_f)).cast<int>();//element-wise floor
    Vec fx = p.x * inv_dx - base_coord.cast<real>();
    // Quadratic kernels  [http://mpm.graphics   Eqn. 123, with x=fx, fx-1,fx-2]
    Vec w[3]{Vec(0.5) * sqr(Vec(1.5) - fx), Vec(0.75) - sqr(fx - Vec(1.0)),
             Vec(0.5) * sqr(fx - Vec(0.5))};
    auto e = std::exp(hardening * (1.0_f - p.Jp)), mu=mu_0*e, lambda=lambda_0*e;
    real J = determinant(p.F);         //                         Current volume
    Mat r, s; polar_decomp(p.F, r, s); //Polar decomp. for fixed corotated model
    auto stress =                           // Cauchy stress times dt and inv_dx
        -4*inv_dx*inv_dx*dt*vol*(2*mu*(p.F-r) * transposed(p.F)+lambda*(J-1)*J);
    auto affine = stress+particle_mass*p.C;
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) { // Scatter to grid
        auto dpos = (Vec(i, j) - fx) * dx;
        Vector3 mv(p.v * particle_mass, particle_mass); //translational momentum
        grid[base_coord.x + i][base_coord.y + j] +=
            w[i].x*w[j].y * (mv + Vector3(affine*dpos, 0));
      }
  }
  for(int i = 0; i <= n; i++) for(int j = 0; j <= n; j++) { //For all grid nodes
      auto &g = grid[i][j];
      if (g[2] > 0) {                                // No need for epsilon here
        g /= g[2];                                   //        Normalize by mass
        g += dt * Vector3(0, -200, 0);               //                  Gravity
        real boundary=0.05,x=(real)i/n,y=real(j)/n; //boundary thick.,node coord
        if (x < boundary||x > 1-boundary||y > 1-boundary) g=Vector3(0); //Sticky
        if (y < boundary) g[1] = std::max(0.0_f, g[1]);             //"Separate"
      }
    }
  for (auto &p : particles) {                                // Grid to particle
    Vector2i base_coord=(p.x*inv_dx-Vec(0.5_f)).cast<int>();//element-wise floor
    Vec fx = p.x * inv_dx - base_coord.cast<real>();
    Vec w[3]{Vec(0.5) * sqr(Vec(1.5) - fx), Vec(0.75) - sqr(fx - Vec(1.0)),
             Vec(0.5) * sqr(fx - Vec(0.5))};
    p.C = Mat(0); p.v = Vec(0);
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
        auto dpos = (Vec(i, j) - fx),
            grid_v = Vec(grid[base_coord.x + i][base_coord.y + j]);
        auto weight = w[i].x * w[j].y;
        p.v += weight * grid_v;                                      // Velocity
        p.C += 4 * inv_dx * Mat::outer_product(weight * grid_v, dpos); // APIC C
      }
    p.x += dt * p.v;                                                // Advection
    auto F = (Mat(1) + dt * p.C) * p.F;                      // MLS-MPM F-update
    Mat svd_u, sig, svd_v; svd(F, svd_u, sig, svd_v);
    for (int i = 0; i < 2 * int(plastic); i++)                // Snow Plasticity
      sig[i][i] = clamp(sig[i][i], 1.0_f - 2.5e-2_f, 1.0_f + 7.5e-3_f);
    real oldJ = determinant(F); F = svd_u * sig * transposed(svd_v);
    real Jp_new = clamp(p.Jp * oldJ / determinant(F), 0.6_f, 20.0_f);
    p.Jp = Jp_new; p.F = F;
  }
}
void add_object(Vec center, int c) {   // Seed particles with position and color
  for (int i = 0; i < 500; i++)  // Randomly sample 1000 particles in the square
    particles.push_back(Particle((Vec::rand()*2.0_f-Vec(1))*0.08_f + center, c));
}
/***********************************(1)*****************************************/
void add_object_circle(Vec center, real radius, int c, int num = 500, Vec velocity = Vec(0.0_f))
//input: circle center & radius, color, number of particles, initial velocity
{
	int i = 0;
	while (i < num) {
		auto pos = (Vec::rand()*2.0_f - Vec(1))*radius;
		if (pos.x*pos.x + pos.y*pos.y < radius * radius) {
			particles.push_back(Particle(pos + center, c, velocity));
			i++;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
int main() {
  GUI gui("Real-time 2D MLS-MPM", window_size, window_size);
/***********************************(2)*****************************************/
  add_object_circle(Vec(0.15, 0.65), 0.06, 0xFFFAFA, 1000, Vec(10.0, 0.0));
  add_object_circle(Vec(0.85, 0.65), 0.06, 0xFFFAFA, 1000, Vec(-10.0, 0.0));
////////////////////////////////////////////////////////////////////////////////
  auto &canvas = gui.get_canvas();int f=0;
  for (int i = 0;; i++) {                              //              Main Loop
    advance(dt);                                       //     Advance simulation
    if (i % int(frame_dt / dt) == 0) {                 //        Visualize frame
      canvas.clear(0x112F41);                          //       Clear background
      canvas.rect(Vec(0.04), Vec(0.96)).radius(2).color(0x4FB99F).close();// Box
      for(auto p:particles)canvas.circle(p.x).radius(2).color(p.c);//Particles
      gui.update();                                              // Update image
      //canvas.img.write_as_image(fmt::format("tmp/{:05d}.png", f++));
    }
  }
} //----------------------------------------------------------------------------
//This sample shows how to generate particles in a circle region and set other 
//parameters like velocity and particle numbers. Follow the comment marks in the
// code to see where to make changes
//
//(1) Use function add_object_circle() to sample particles in circle regions
//(2) Sample two snow balls and set their parameters respectively