#include "kalman.h"



void Kalman_Init(Kalman_t *k)
{
    k->angle = 0.0f;
    k->bias  = 0.0f;

    k->P[0][0] = 0.0f;
    k->P[0][1] = 0.0f;
    k->P[1][0] = 0.0f;
    k->P[1][1] = 0.0f;

    k->Q_angle   = 0.001f;
    k->Q_bias    = 0.003f;
    k->R_measure = 0.03f;
}

float Kalman_GetAngle(Kalman_t *k,
                      float accAngle,   // Góc từ accel (deg)
                      float gyroRate,   // Tốc độ góc từ gyro (deg/s)
                      float dt)         // Chu kỳ lấy mẫu (s)
{
    // ===== DỰ ĐOÁN =====
    k->rate  = gyroRate - k->bias;
    k->angle += dt * k->rate;

    // Cập nhật ma trận P
    k->P[0][0] += dt * (dt*k->P[1][1] - k->P[0][1] - k->P[1][0] + k->Q_angle);
    k->P[0][1] -= dt * k->P[1][1];
    k->P[1][0] -= dt * k->P[1][1];
    k->P[1][1] += k->Q_bias * dt;

    // ===== HIỆU CHỈNH =====
    float S = k->P[0][0] + k->R_measure; // Innovation covariance
    float K0 = k->P[0][0] / S;
    float K1 = k->P[1][0] / S;

    float y = accAngle - k->angle;       // Sai số đo

    k->angle += K0 * y;
    k->bias  += K1 * y;

    float P00_temp = k->P[0][0];
    float P01_temp = k->P[0][1];

    k->P[0][0] -= K0 * P00_temp;
    k->P[0][1] -= K0 * P01_temp;
    k->P[1][0] -= K1 * P00_temp;
    k->P[1][1] -= K1 * P01_temp;

    return k->angle;
}
