//Robot.cpp

//Includes
#include <cmath>
#include "WPILib.h"
#include "Values.h"
#include "Meccanum.h"

//Main Class
class Robot : public SimpleRobot {
private:
	Joystick rJoy;
	Joystick lJoy;
	Joystick shooterJoy;
	DriverStationLCD * dslcd;

	Victor frMotor;
	Victor flMotor;
	Victor brMotor;
	Victor blMotor;
	Encoder frEnc;
	Encoder flEnc;
	Encoder brEnc;
	Encoder blEnc;
	PIDVelocityMotor fr;
	PIDVelocityMotor fl;
	PIDVelocityMotor br;
	PIDVelocityMotor bl;

	Meccanum drive;
	double forward;
	double right;
	double clockwise;
	double scalar;
	RobotDrive rawDrive;
	bool useEncoders;

	Victor shooter;
	DigitalInput shooterLimit;
	Victor rIntake;
	Victor lIntake;
	Victor rIntakeSlider;
	Victor lIntakeSlider;
	Victor gate;

	bool check;
	bool setup;
	bool shoot;
	bool move;
	double time;
	AnalogChannel photosensor;

public:
	Robot(void): rJoy(RIGHT_JOYSTICK),
				 lJoy(LEFT_JOYSTICK),
				 shooterJoy(SHOOTER_JOYSTICK),
				 dslcd(DriverStationLCD::GetInstance()),
				 frMotor(DRIVE_VICTOR_SIDECARS[0], FR_DRIVE_VICTOR),
				 flMotor(DRIVE_VICTOR_SIDECARS[1], FL_DRIVE_VICTOR),
				 brMotor(DRIVE_VICTOR_SIDECARS[2], BR_DRIVE_VICTOR),
				 blMotor(DRIVE_VICTOR_SIDECARS[3], BL_DRIVE_VICTOR),
				 frEnc(FR_DRIVE_ENCODER_SIDECAR, FR_DRIVE_ENCODER_A, FR_DRIVE_ENCODER_SIDECAR, FR_DRIVE_ENCODER_B, FR_DRIVE_ENCODER_B),
				 flEnc(FL_DRIVE_ENCODER_SIDECAR, FL_DRIVE_ENCODER_A, FL_DRIVE_ENCODER_SIDECAR, FL_DRIVE_ENCODER_B, FL_DRIVE_ENCODER_B),
				 brEnc(BR_DRIVE_ENCODER_SIDECAR, BR_DRIVE_ENCODER_A, BR_DRIVE_ENCODER_SIDECAR, BR_DRIVE_ENCODER_B, BR_DRIVE_ENCODER_B),
				 blEnc(BL_DRIVE_ENCODER_SIDECAR, BL_DRIVE_ENCODER_A, BL_DRIVE_ENCODER_SIDECAR, BL_DRIVE_ENCODER_B, BL_DRIVE_ENCODER_B),
				 fr("FR", frMotor, frEnc, FR_DRIVE_PID),
				 fl("FL", flMotor, flEnc, FL_DRIVE_PID),
				 br("BR", brMotor, brEnc, BR_DRIVE_PID),
				 bl("BL", blMotor, blEnc, BL_DRIVE_PID),
				 drive(fr, fl, br, bl),
				 forward(0.0),
				 right(0.0),
				 clockwise(0.0),
				 scalar(1.0),
				 rawDrive(flMotor, blMotor, frMotor, brMotor),
				 useEncoders(true),
				 shooter(SHOOTER_VICTOR_SIDECAR, SHOOTER_VICTOR),
				 shooterLimit(SHOOTER_LIMIT_SIDECAR, SHOOTER_LIMIT),
				 rIntake(INTAKE_VICTOR_SIDECARS[0], R_INTAKE_VICTOR),
				 lIntake(INTAKE_VICTOR_SIDECARS[1], L_INTAKE_VICTOR),
				 rIntakeSlider(INTAKE_SLIDER_SIDECARS[0], R_INTAKE_SLIDER),
				 lIntakeSlider(INTAKE_SLIDER_SIDECARS[1], L_INTAKE_SLIDER),
				 gate(GATE_VICTOR_SIDECAR, GATE_VICTOR),
				 check(false),
				 setup(false),
				 shoot(false),
				 move(false),
				 time(GetTime()),
				 photosensor(GOAL_PHOTOSENSOR_MODULE, GOAL_PHOTOSENSOR)
	{
		rawDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);
		rawDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		rawDrive.SetSafetyEnabled(false);
		frEnc.SetDistancePerPulse(FR_DRIVE_ENCODER_DISTANCE_PER_PULSE);
		flEnc.SetDistancePerPulse(FL_DRIVE_ENCODER_DISTANCE_PER_PULSE);
		brEnc.SetDistancePerPulse(BR_DRIVE_ENCODER_DISTANCE_PER_PULSE);
		blEnc.SetDistancePerPulse(BL_DRIVE_ENCODER_DISTANCE_PER_PULSE);
		frEnc.SetReverseDirection(FR_DRIVE_ENCODER_REVERSE);
		flEnc.SetReverseDirection(FL_DRIVE_ENCODER_REVERSE);
		brEnc.SetReverseDirection(BR_DRIVE_ENCODER_REVERSE);
		blEnc.SetReverseDirection(BL_DRIVE_ENCODER_REVERSE);
		frEnc.Start();
		flEnc.Start();
		brEnc.Start();
		blEnc.Start();
		//photosensor.SetLimitsVoltage(GOAL_PHOTOSENSOR_LOWER_BOUND, GOAL_PHOTOSENSOR_UPPER_BOUND);
	}

	/**
	 * Kill the watchdog so it doesn't interfere with the rest of the program.
	 */
	void RobotInit(void) {
		Watchdog().SetEnabled(false);
	}

	/**
	 * Print a message stating that the robot is disabled.
	 */
	void Disabled(void) {
		printf("Robot is disabled!\n");
	}

	/**
	 * Wait for the hot goal, shoot, then drive forward for 2 seconds.
	 */
	void Autonomous(void) {
		printf("Autonomous mode enabled!\n");
		setup = true;
		check = false;
		move = false;
		shoot = false;
		time = GetTime();
		printf("Check: %s, Setup: %s, Shoot: %s, Move: %s, LastTime: %f, CurrentTime: %f\n", check ? "T" : "F", setup ? "T" : "F", shoot ? "T" : "F", move ? "T" : "F", time, GetTime());
		while(IsEnabled() && IsAutonomous()) {
			dslcd->PrintfLine(DriverStationLCD::kUser_Line6, "Photo: %f", photosensor.GetVoltage());
			dslcd->UpdateLCD();
			if(setup) {
				gate.SetSpeed(0.4);
				if(useEncoders) {
					drive.update(0.5, 0.0, 0.0);
				} else {
					rawDrive.MecanumDrive_Cartesian(0.0, 0.5, 0.0);
				}
				//rIntake.SetSpeed(-1.0);
				//lIntake.SetSpeed(1.0);
				//rIntakeSlider.SetSpeed(0.5);
				//lIntakeSlider.SetSpeed(0.5);
				if(GetTime() - time >= 0.25) {
					gate.SetSpeed(0.0);
					Wait(0.01);
					if(useEncoders) {
						drive.update(0.0, 0.0, 0.0);
					} else {
						rawDrive.MecanumDrive_Cartesian(0.0, 0.0, 0.0);
					}
					//rIntakeSlider.SetSpeed(0.0);
					//lIntakeSlider.SetSpeed(0.0);
					setup = false;
					check = true;
					time = GetTime();
					printf("Check: %s, Setup: %s, Shoot: %s, Move: %s, LastTime: %f, CurrentTime: %f\n", check ? "T" : "F", setup ? "T" : "F", shoot ? "T" : "F", move ? "T" : "F", time, GetTime());
				} else {
					Wait(0.005);
				}
			}
			if(check) {
				if(photosensor.GetVoltage() > GOAL_PHOTOSENSOR_LOWER_BOUND && photosensor.GetVoltage() < GOAL_PHOTOSENSOR_UPPER_BOUND) {
					Wait(5.5);
				}
				check = false;
				move = true;
				time = GetTime();
				printf("Check: %s, Setup: %s, Shoot: %s, Move: %s, LastTime: %f, CurrentTime: %f\n", check ? "T" : "F", setup ? "T" : "F", shoot ? "T" : "F", move ? "T" : "F", time, GetTime());
			}
			if(move) {
				if(useEncoders) {
					drive.update(0.5, 0.0, 0.0);
				} else {
					rawDrive.MecanumDrive_Cartesian(0.0, 0.5, 0.0);
				}
				if(GetTime() - time >= 1.0) {
					Wait(0.01);
					if(useEncoders) {
						drive.update(0.0, 0.0, 0.0);
					} else {
						rawDrive.MecanumDrive_Cartesian(0.0, 0.0, 0.0);
					}
					move = false;
					shoot = true;
					printf("Check: %s, Setup: %s, Shoot: %s, Move: %s, LastTime: %f, CurrentTime: %f\n", check ? "T" : "F", setup ? "T" : "F", shoot ? "T" : "F", move ? "T" : "F", time, GetTime());
				} else {
					Wait(0.005);
				}
			}
			if(shoot) {
				shooter.SetSpeed(-1.0);
				if(GetTime() - time >= 3.25) {
					shooter.SetSpeed(0.0);
					//rIntake.SetSpeed(0.0);
					//lIntake.SetSpeed(0.0);
					shoot = false;
					time = GetTime();
					printf("Check: %s, Setup: %s, Shoot: %s, Move: %s, LastTime: %f, CurrentTime: %f\n", check ? "T" : "F", setup ? "T" : "F", shoot ? "T" : "F", move ? "T" : "F", time, GetTime());
				} else {
					Wait(0.005);
				}
			}
		}
	}

	/**
	 * Run PID Meccanum drive and shooter controls.
	 */
	void OperatorControl(void) {
		printf("Operator control enabled!\n");
		while(IsEnabled() && IsOperatorControl()) {
			if(fabs(rJoy.GetRawAxis(2)) < 0.2) {
				forward = 0.0;
			} else {
				forward = rJoy.GetRawAxis(2);
				forward *= fabs(forward);
				forward *= scalar;
			}
			if(fabs(rJoy.GetRawAxis(1)) < 0.2) {
				right = 0.0;
			} else {
				right = rJoy.GetRawAxis(1);
				right *= fabs(right);
				right *= scalar;
			}
			if(fabs(lJoy.GetRawAxis(1)) < 0.2) {
				clockwise = 0.0;
			} else {
				clockwise = lJoy.GetRawAxis(1);
				//if(fabs(clockwise) <= 0.5) {
				//	clockwise *= 0.5;
				//} else {
					//clockwise *= fabs(clockwise);
					//if(!rJoy.GetRawButton(4) && !lJoy.GetRawButton(4) && !rJoy.GetRawButton(5) && !lJoy.GetRawButton(5)) {
					//	clockwise *= 0.75;
					//}
				//}
				clockwise *= scalar;
			}
			if(rJoy.GetRawButton(1) || lJoy.GetRawButton(1)) {
				forward = 0.0;
				right = 0.0;
				clockwise = 0.0;
			}
			if(rJoy.GetRawButton(2) && lJoy.GetRawButton(2)) {
				useEncoders = true;
			}
			if(rJoy.GetRawButton(3) && lJoy.GetRawButton(3)) {
				useEncoders = false;
			}
			if(useEncoders) {
				drive.update(forward, right, clockwise);
							//Forward  Right  Clockwise
			} else {
				rawDrive.MecanumDrive_Cartesian(right, forward, clockwise);
			}
			//printf("Forward: %f,\tRight: %f,\tClockwise: %f\n", forward, right, clockwise);
			dslcd->PrintfLine(DriverStationLCD::kUser_Line1, "Forward: %f", forward);
			dslcd->PrintfLine(DriverStationLCD::kUser_Line2, "Right: %f", right);
			dslcd->PrintfLine(DriverStationLCD::kUser_Line3, "Clockwise: %f", clockwise);
			dslcd->PrintfLine(DriverStationLCD::kUser_Line4, "Time: %f", GetTime());
			dslcd->UpdateLCD();

			if(shooterJoy.GetRawButton(1)) {
				shooter.SetSpeed(-1.0);
			} else if(shooterLimit.Get() == 0) {
				shooter.SetSpeed(-0.5);
			} else {
				shooter.SetSpeed(0.0);
			}
			if(fabs(shooterJoy.GetRawAxis(2)) < 0.2) {
				rIntake.SetSpeed(0.0);
				lIntake.SetSpeed(0.0);
			} else {
				rIntake.SetSpeed(shooterJoy.GetRawAxis(2));
				lIntake.SetSpeed(-shooterJoy.GetRawAxis(2));
			}
			if(shooterJoy.GetRawButton(3)) {
				rIntakeSlider.SetSpeed(0.5);
				lIntakeSlider.SetSpeed(0.5);
			} else if(shooterJoy.GetRawButton(2)){
				rIntakeSlider.SetSpeed(-0.5);
				lIntakeSlider.SetSpeed(-0.5);
			} else {
				rIntakeSlider.SetSpeed(0.0);
				lIntakeSlider.SetSpeed(0.0);
			}
			if(shooterJoy.GetRawButton(6)) {
				gate.SetSpeed(0.4);
			} else if(shooterJoy.GetRawButton(7)) {
				gate.SetSpeed(-0.4);
			} else {
				gate.SetSpeed(0.0);
			}

			Wait(0.005);
		}
	}

	/**
	 * Update SmartDashboard while in test mode.
	 */
	void Test() {
		printf("Test mode enabled!\n");
		while(IsTest()) {
			LiveWindow::GetInstance()->Run();
		}
	}
};

START_ROBOT_CLASS(Robot);
