#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

const float G = 6.67430e-11f;

class CelestialBody {
public:
    double x, y, z;
    double gravityAcceleration;
    double radius;
    double mass;

    double velocityX, velocityY, velocityZ;
    double sqrDstX, sqrDstY, sqrDstZ, sqrDst;
    double forceDirX, forceDirY, forceDirZ;
    double accelerationX, accelerationY, accelerationZ;

    CelestialBody(double x, double y, double z, double gravityAcceleration, double radius, double velocityX, double velocityY, double velocityZ) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->gravityAcceleration = gravityAcceleration;
        this->radius = radius;
        this->velocityX = velocityX;
        this->velocityY = velocityY;
        this->velocityZ = velocityZ;
        this->mass = calculateMass();
    }

    double calculateMass() {
        return gravityAcceleration * radius * radius / G;
    }

    void updateVelocity(std::vector<CelestialBody> &bodies, double timeStep) {
        for (CelestialBody &body : bodies) {
            if (&body == this) continue;
            sqrDstX = (body.x - x) * (body.x - x);
            sqrDstY = (body.y - y) * (body.y - y);
            sqrDstZ = (body.z - z) * (body.z - z);
            sqrDst = sqrDstX + sqrDstY + sqrDstZ;
            double distance = std::sqrt(sqrDst);

            if (sqrDst < 1e-6) continue;

            forceDirX = (body.x - x) / distance;
            forceDirY = (body.y - y) / distance;
            forceDirZ = (body.z - z) / distance;

            accelerationX = forceDirX * G * body.mass / sqrDst;
            accelerationY = forceDirY * G * body.mass / sqrDst;
            accelerationZ = forceDirZ * G * body.mass / sqrDst;

            velocityX += accelerationX * timeStep;
            velocityY += accelerationY * timeStep;
            velocityZ += accelerationZ * timeStep;
        }
    }

    void updatePosition(double timeStep) {
        x += velocityX * timeStep;
        y += velocityY * timeStep;
        z += velocityZ * timeStep;
    }
};

int main() {
    // TODO: Add other inner solar system planets.
    CelestialBody sun(0.0, 0.0, 0.0, 274.049, 696342000, 0.0, 0.0, 0.0);
    CelestialBody earth(-1.486944805217578e+11, 7.351868643006310e+5, 7.369147140679703e+9, 9.81, 6371000.0, -1.964907292562942e+3, 2.574832151630346, -2.985411737006705e+4);
    CelestialBody moon(-1.490211701992313e+11, -2.296264785626251e+7, 7.129696314336223e+9, 1.62, 1737400.0, -1.388692338777403e+3, -6.642270062609512e-2, -3.063025643677688e+4);

    std::vector<CelestialBody> bodies;
    bodies.emplace_back(0.0, 0.0, 0.0, 274.049, 696342000, 0.0, 0.0, 0.0);
    bodies.emplace_back(-1.486944805217578e+11, 7.351868643006310e+5, 7.369147140679703e+9, 9.81, 6371000.0, -1.964907292562942e+3, 2.574832151630346, -2.985411737006705e+4);
    bodies.emplace_back(-1.490211701992313e+11, -2.296264785626251e+7, 7.129696314336223e+9, 1.62, 1737400.0, -1.388692338777403e+3, -6.642270062609512e-2, -3.063025643677688e+4);

    double timeStepIn = 0.1;
    double timeStepOut = 1.0;

    for (int step = 0; step < 1000000; ++step) {
        for (int i = 0; i < static_cast<int>(timeStepOut / timeStepIn); i++) {
            for (auto &body : bodies) {
                body.updateVelocity(bodies, timeStepIn);
            }

            for (auto &body : bodies) {
                body.updatePosition(timeStepIn);
            }
        }

        double distanceEM = std::sqrt(((bodies[1].x - bodies[2].x) * (bodies[1].x - bodies[2].x)) + ((bodies[1].y - bodies[2].y) * (bodies[1].y - bodies[2].y)) + ((bodies[1].z - bodies[2].z) * (bodies[1].z - bodies[2].z))); 
        double distanceEM_KM = distanceEM / 1000;

        double distanceES = std::sqrt(((bodies[0].x - bodies[1].x) * (bodies[0].x - bodies[1].x)) + ((bodies[0].y - bodies[1].y) * (bodies[0].y - bodies[1].y)) + ((bodies[0].z - bodies[1].z) * (bodies[0].z - bodies[1].z))); 
        double distanceES_KM = distanceES / 1000;

        double moonPosXe = bodies[2].x - bodies[1].x;
        double moonPosYe = bodies[2].y - bodies[1].y;
        double moonPosZe = bodies[2].z - bodies[1].z;

        std::cout << "Distance between Earth and Moon: " << distanceEM << " meters, or " << distanceEM_KM << " km.\n";
        std::cout << "Distance between Earth and Sun: " << distanceES << " meters, or " << distanceES_KM << " km.\n";
        std::cout << "Earth Position: (X: " << bodies[1].x << " m, Y (vertical): " << bodies[1].y << " m, Z: " << bodies[1].z << " m)\n";
        std::cout << "Moon position relative to the Sun: (X: " << bodies[2].x << " m, Y (vertical): " << bodies[2].y << " m, Z: " << bodies[2].z << " m)\n";
        std::cout << "Moon position relative to Earth: (X: " << moonPosXe << " m, Y (vertical): " << moonPosYe << " m, Z: " << moonPosZe << " m)\n";
        std::cout << "--------------------------------------------------------------------------------------------------------------\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}