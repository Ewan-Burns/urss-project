#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <chrono>

struct Particle {
    char label;             // Unique alphabeltical label for the particle
    double position[2];     // Position (x, y)
    double velocity[2];     // Velocity (vx, vy)
    double acceleration[2]; // Acceleration (ax, ay)
    double accNext[2];       // Next acceleration (ax', ay')
    bool wrapX;          // Flag to indicate if particle has wrapped around in X axis
    bool wrapY;          // Flag to indicate if particle has wrapped around in Y axis
    char active;         // Flag to indicate if the particle has crossed a boundary and moved to a new vector
};

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

int main() {

    srand(static_cast<unsigned int>(time(nullptr)));    // Initialize random seed

    // Create particles with unique labels
    std::vector<Particle> particles;
    for (char label = 'A'; label <= 'J'; ++label) {
        Particle particle;
        particle.label = label;
        particle.position[0] = genRN(0.0, 1.0);
        particle.position[1] = genRN(0.0, 1.0);
        particle.velocity[0] = genRN(-0.1, 0.1);
        particle.velocity[1] = genRN(-0.1, 0.1);
        particle.active = 0;
        particles.push_back(particle);
    }

    // Time step
    double dt = 0.01;

    // Creates and opens file to store particle locations
    std::ofstream positionFile("particle-positions.txt");

    // Starting the runtime clock
    auto runtimeStart = std::chrono::high_resolution_clock::now();

    // Move particles for 100 iterations
    for (int i = 0; i < 5000; ++i) {
        std::cout << i << "\n";
        moveParticles(particles, dt);

        // Writes particle positions to "particle-positions.txt"
        int inactiveCount = 0;
        for (auto& particle : particles) {
            positionFile << particle.label << " " << particle.position[0] << " " << particle.position[1];
            if (particle.wrapX) positionFile << "  (Wrapped-X)";
            if (particle.wrapY) positionFile << "  (Wrapped-Y)";
            if (particle.wrapX || particle.wrapY) {
                particle.active = 1;
                positionFile << "  inactive";
                ++inactiveCount;
            }
            positionFile << "\n";
        }

        // New vector for storing copies of the particles that crossed a boundary
        std::vector<Particle> tempvec;
        tempvec.reserve(inactiveCount); // Memory reserve for the number of particles that crossed a boundary

        // Copy particles that crossed the boundary to the tempvec vector
        std::copy_if(particles.begin(), particles.end(), std::back_inserter(tempvec),
            [](Particle& p) { bool b = (p.active==1); if (b) p.active = 2; return b;});

        // Copy particles back to the main particlevec and set their "active" flags back to true
        for (auto& particle : tempvec) {
            particle.active = 0;
            particles.push_back(particle);
        }

        // Periodically erase inactive particles from the particlevec
        const int N = 10; // Adjust N as needed
        if (i % N == 0) {
            std::erase_if (particles,[](const Particle& p) { return (p.active != 0); });
//            particles.erase(std::remove_if(particles.begin(), particles.end(),
//                [](const Particle& p) { return !p.active; }), particles.end());
        }
        std::cout << "aaa " << particles.size() << "\n";
    }

    auto runtimeEnd = std::chrono::high_resolution_clock::now();
    auto runtimeTotal = std::chrono::duration_cast<std::chrono::milliseconds>(runtimeEnd - runtimeStart).count();

    std::cout << "Simulation Runtime: " << runtimeTotal << " ms\n";

    // Close "particle-positions.txt"
    positionFile.close();

    return 0;
}