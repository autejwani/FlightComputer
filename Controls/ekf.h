#ifndef EKF_H
#define EKF_H

class EKF {
	public:
		EKF();

		void init(float ground_altitude);
		void predict(float ax_g, float ay_g, float az_g);
		void updateBaro(float altitude_m);

		float getAltitude();
		float getVelocity();

	private:
		float z;      // Altitude (m)
		float vz;     // Vertical velocity (m/s)
		float P_z;    // Altitude variance
		float P_vz;   // Velocity variance

		float dt;
		float Q_z;
		float Q_vz;
		float R_baro;
};

#endif
