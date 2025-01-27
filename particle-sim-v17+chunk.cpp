#include <iostream>
#include <list>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "chunk_list.h"

enum ActiveState {
    Active,
    Removing,
    Removed
};

struct Particle {
    char label;             // Unique alphabetical label for the particle
    double position[2];     // Position (x, y)
    double velocity[2];     // Velocity (vx, vy)
    double acceleration[2]; // Acceleration (ax, ay)
    double accNext[2];      // Next acceleration (ax', ay')
    bool wrapX;             // Flag to indicate if particle has wrapped around in X axis
    bool wrapY;             // Flag to indicate if particle has wrapped around in Y axis
    ActiveState active;     // Flag to indicate if the particle has crossed a boundary and moved to a new vector
};

int N = 1; // Number of iterations between erasing particles

double genRN(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

void moveParticles(chunk_list<Particle>& particles, double dt, int iteration) {
    int inactiveCount = 0; // Count of inactive particles

    chunk_list<Particle> tempvec; // Change to std::vector

    for (auto it = particles.begin(); it != particles.end();) {
        Particle& particle = *it;

        if (particle.active != Active) {
            it = particles.erase(it); // Erase and get the next iterator
            inactiveCount++;
        } else {
            particle.wrapX = false;
            particle.wrapY = false;

            particle.position[0] += particle.velocity[0] * dt + 0.5 * particle.acceleration[0] * dt * dt;
            particle.position[1] += particle.velocity[1] * dt + 0.5 * particle.acceleration[1] * dt * dt;

            if (particle.position[0] < 0) {
                particle.position[0] += 1;
                particle.wrapX = true;
            }
            if (particle.position[0] >= 1) {
                particle.position[0] -= 1;
                particle.wrapX = true;
            }
            if (particle.position[1] < 0) {
                particle.position[1] += 1;
                particle.wrapY = true;
            }
            if (particle.position[1] >= 1) {
                particle.position[1] -= 1;
                particle.wrapY = true;
            }

            particle.velocity[0] += 0.5 * (particle.acceleration[0] + particle.accNext[0]) * dt;
            particle.velocity[1] += 0.5 * (particle.acceleration[1] + particle.accNext[1]) * dt;

            particle.acceleration[0] = particle.accNext[0];
            particle.acceleration[1] = particle.accNext[1];

            if (particle.wrapX || particle.wrapY) {
                tempvec.push_back(particle);
                particle.active = Removing;
                inactiveCount++;
            }

            ++it;
        }
    }
    // Copy particles back to the main particle list and set their "active" flags back to true
    particles.insert(particles.end(), tempvec.begin(), tempvec.end());
    
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N" << "\n";
        exit(1);
    }
    N = std::atoi(argv[1]);
    srand(1691169547); // Set fixed seed for random number generation

    // Create particles with unique labels
    chunk_list<Particle> particles;
    for (int i = 0; i < 10000; i++) {
        Particle particle;
        particle.position[0] = genRN(0.0, 1.0);
        particle.position[1] = genRN(0.0, 1.0);
        particle.velocity[0] = genRN(-0.1, 0.1);
        particle.velocity[1] = genRN(-0.1, 0.1);
        particle.acceleration[0] = 0.0;
        particle.acceleration[1] = 0.0;
        particle.accNext[0] = 0.0;
        particle.accNext[1] = 0.0;
        particle.active = Active; // Initialized to Active 
        particles.push_back(particle);
    }

    // Time step
    double dt = 0.01;

    std::ofstream positionFile("particle-positions.txt");

    // Starting the runtime clock
    auto runtimeStart = std::chrono::high_resolution_clock::now();

    // Move particles for 100 iterations
    for (int i = 0; i < 100000; ++i) {
        moveParticles(particles, dt, i);

        #ifdef DEBUG
        if (particles.size() > 3000) exit(1);
        #endif

        // Writes particle positions to "particle-positions.txt"
        for (const auto& particle : particles) {
            if (particle.active != Active) continue;
            #ifdef DEBUG
            positionFile << particle.label << " " << particle.position[0] << " " << particle.position[1];
            if (particle.wrapX) positionFile << "  (Wrapped-X)";
            if (particle.wrapY) positionFile << "  (Wrapped-Y)";
            positionFile << "\n";
            #endif
        }
    }

    auto runtimeEnd = std::chrono::high_resolution_clock::now();
    auto runtimeTotal = std::chrono::duration_cast<std::chrono::milliseconds>(runtimeEnd - runtimeStart).count();

    std::cout << "Simulation Runtime: " << runtimeTotal << " ms\n";

    // Close "particle-positions.txt"
    positionFile.close();

    return 0;
}