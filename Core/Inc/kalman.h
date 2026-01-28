typedef struct {
    float angle;      // Góc ước lượng (deg)
    float bias;       // Độ lệch gyro (deg/s)
    float rate;       // Tốc độ góc sau khi trừ bias

    float P[2][2];    // Ma trận hiệp phương sai

    float Q_angle;    // Nhiễu quá trình của góc
    float Q_bias;     // Nhiễu quá trình của bias
    float R_measure;  // Nhiễu đo (accel)
} Kalman_t;

float Kalman_GetAngle(Kalman_t *k,float accAngle,float gyroRate, float dt);
float Kalman_GetAngle(Kalman_t *k,float accAngle,float gyroRate, float dt);
