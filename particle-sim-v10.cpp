#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <chrono>

enum ActiveState {
    Active,
    Removing,
    Removed
};

struct Particle {
    char label;             // Unique alphabeltical label for the particle
    double position[2];     // Position (x, y)
    double velocity[2];     // Velocity (vx, vy)
    double acceleration[2]; // Acceleration (ax, ay)
    double accNext[2];       // Next acceleration (ax', ay')
    bool wrapX;          // Flag to indicate if particle has wrapped around in X axis
    bool wrapY;          // Flag to indicate if particle has wrapped around in Y axis
    ActiveState active;         // Flag to indicate if the particle has crossed a boundary and moved to a new vector
};

//Number of iterations between erase of particles
int N=1;

double genRN(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

void moveParticles(std::vector<Particle>& particles, double dt) {
    for (auto& particle : particles) {
        
        if (particle.active != 0) continue;

        particle.wrapX = false;          //Clear wrapping flags at the beginning of each iteration
        particle.wrapY = false;

        //Update position
        particle.position[0] += particle.velocity[0] * dt + 0.5 * particle.acceleration[0] * dt * dt;
        particle.position[1] += particle.velocity[1] * dt + 0.5 * particle.acceleration[1] * dt * dt;

        if (particle.position[0] < 0) {
            particle.position[0] += 1;          //Apply periodic boundary conditions in X direction
            particle.wrapX = true;
        }
        if (particle.position[0] >= 1) {
            particle.position[0] -= 1;
            particle.wrapX = true;
        }
        if (particle.position[1] < 0) {
            particle.position[1] += 1;          //Apply periodic boundary conditions in Y direction
            particle.wrapY = true;
        }
        if (particle.position[1] >= 1) {
            particle.position[1] -= 1;
            particle.wrapY = true;
        }

        particle.velocity[0] += 0.5 * (particle.acceleration[0] + particle.accNext[0]) * dt;       //Update velocity
        particle.velocity[1] += 0.5 * (particle.acceleration[1] + particle.accNext[1]) * dt;

        
        particle.acceleration[0] = particle.accNext[0];          //Update acceleration
        particle.acceleration[1] = particle.accNext[1];
    }
}

int main(int argc, char** argv) {
    if (argc!=2) {
      std::cerr << "Usage : N " << "\n";
      exit(1);
    }
    N = std::atoi(argv[1]);
    srand(1691169547); // Set fixed seed for random number generation

    // Create particles with unique labels
    std::vector<Particle> particles;
    //for (char label = 'A'; label <= 'J'; ++label) {
    for (int i = 0; i < 10000; i++) {
        Particle particle;
        //particle.label = label;
        particle.position[0] = genRN(0.0, 1.0);
        particle.position[1] = genRN(0.0, 1.0);
        particle.velocity[0] = genRN(-0.1, 0.1);
        particle.velocity[1] = genRN(-0.1, 0.1);
	particle.acceleration[0] = 0.0;
	particle.acceleration[1] = 0.0;
	particle.accNext[0]=0.0;
	particle.accNext[1]=0.0;
        particle.active = Active;   //Initialised to Active 
        particles.push_back(particle);
    }

    // Time step
    double dt = 0.01;

    std::ofstream positionFile("particle-positions.txt");

    int inactiveCount = 0;

    // Starting the runtime clock
    auto runtimeStart = std::chrono::high_resolution_clock::now();

    // Move particles for 100 iterations
    for (int i = 0; i < 100000; ++i) {
        moveParticles(particles, dt);
        #ifdef DEBUG
        if (particles.size() > 3000) exit(1);
        #endif
            // Writes particle positions to "particle-positions.txt"
            for (auto& particle : particles) {
                if (particle.active != Active) continue;
                #ifdef DEBUG
                positionFile << particle.label << " " << particle.position[0] << " " << particle.position[1];
                if (particle.wrapX) positionFile << "  (Wrapped-X)";
                if (particle.wrapY) positionFile << "  (Wrapped-Y)";
                #endif
                if (particle.wrapX || particle.wrapY) {
                    particle.active = Removing;
                    #ifdef DEBUG
                    positionFile << "  inactive";
                    #endif
                    ++inactiveCount;
                }
                #ifdef DEBUG
                positionFile << "\n";
                #endif
            }

            std::vector<Particle> tempvec; // Define tempvec only once, outside of DEBUG block
            tempvec.reserve(inactiveCount); // Reserve memory for tempvec

            // Copy particles that crossed the boundary to the tempvec vector
            std::copy_if(particles.begin(), particles.end(), std::back_inserter(tempvec),
                [](Particle& p) { if (p.active == Removing) {
                        p.active = Removed;
                        return true;
                    }
                    return false;
                });

            // Copy particles back to the main particlevec and set their "active" flags back to true
            for (auto& particle : tempvec) {
                particle.active = Active;
                particles.push_back(particle);
            }

        // Periodically erase inactive particles from the particlevec
        if (i % N == 0) {
            std::erase_if (particles,[](const Particle& p) { return (p.active != 0); });
//            particles.erase(std::remove_if(particles.begin(), particles.end(),
//                [](const Particle& p) { return p.active == Removed; }), particles.end());
        }
    }

    auto runtimeEnd = std::chrono::high_resolution_clock::now();
    auto runtimeTotal = std::chrono::duration_cast<std::chrono::milliseconds>(runtimeEnd - runtimeStart).count();

    std::cout << "Simulation Runtime: " << runtimeTotal << " ms\n";


    // Close "particle-positions.txt"
    positionFile.close();

    return 0;
}
