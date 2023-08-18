#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>

struct Particle {
    char label;             //Unique alphabeltical label for the particle
    double position[2];     //Position (x, y)
    double velocity[2];     //Velocity (vx, vy)
    double acceleration[2]; //Acceleration (ax, ay)
    double accNext[2];       //Next acceleration (ax', ay')
    bool wrapX;          //Flag to indicate if particle has wrapped around in X axis
    bool wrapY;          //Flag to indicate if particle has wrapped around in Y axis
    bool active;            
};

double genRN(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

void moveParticles(std::vector<Particle>& particles, double dt) {
    for (auto& particle : particles) {
        
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
        particles.push_back(particle);
    }

    // Time step
    double dt = 0.01;

    // Creates and opens file to store particle locations
    std::ofstream positionFile("particle-positions.txt");

    // Move particles for 100 iterations
    for (int i = 0; i < 100; ++i) {
        moveParticles(particles, dt);

        // Writes particle positions to "particle-positions.txt"
        for (const auto& particle : particles) {
            positionFile << particle.label << " " << particle.position[0] << " " << particle.position[1];
            if (particle.wrapX) positionFile << "  (Wrapped-X)";
            if (particle.wrapY) positionFile << "  (Wrapped-Y)";
            positionFile << "\n";
        }
    }

    // Close "particle-positions.txt"
    positionFile.close();

    return 0;
}