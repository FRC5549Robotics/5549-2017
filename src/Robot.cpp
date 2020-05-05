#include <iostream>
#include <memory>
#include <string>
#include <math.h>
#include <WPILib.h>
#include <Joystick.h>
#include <SampleRobot.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>
#include <RobotDrive.h>
#include <Timer.h>
#include <chrono>
#include <thread>
#include <time.h>
#include <SPI.h>
#include <ADXRS450_Gyro.h>





/************FOR XBOX 360 CONTROLLER**********
 *
 * BUTTON NUMBERS
 * ButtonA = 1
 * ButtonB = 2
 * ButtonX = 3
 * ButtonY = 4
 * LeftBumper = 5
 * RightBumper = 6
 * Select = 7
 * Start = 8
 * LeftClick = 9
 * RightClick = 10
 *
 * THUMBSTICK AXIS
 * LeftXAxis = 1 (top is -1, rest is 0, bottom is 1)
 * LeftYAxis = 2
 * TriggerAxis = 3
 *
 * RightXAxis = 4
 * RightYAxis = 5
 *
 */

/*********** PWM Ports **************
 * 0 constIntake
 * 1 leftFront
 * 2 leftBack
 * 3 rightBack
 * 4 rightFront
 * 5 climbTake
 * 6 shooterBall
 * 7
 * 8
 * 9
 */

class Robot: public frc::IterativeRobot {
	public:

		ADXRS450_Gyro gyro;


		Robot() {

			myRobot.SetExpiration(0.1);

			timer.Start();
			CameraServer::GetInstance()->StartAutomaticCapture();
		//	int ledcolor = 0;
			//Wheels
			/*
			Victor *leftFront  = new Victor(1);
			Victor *leftBack = new Victor(2);
			Victor *rightFront = new Victor(4);
			Victor *rightBack = new Victor(3);

			//Components
			Spark *climbTake = new Spark(5);
			Victor *constIntake = new Victor(0);
			*/
		}

private:
	Victor *leftFront  = new Victor(1);
	Victor *leftBack = new Victor(2);
	Victor *rightFront = new Victor(4);
	Victor *rightBack = new Victor(3);

	void leftSide(double x) {
		leftFront->Set(x);
		leftBack->Set(x);
	}
	void rightSide(double x) {
		rightFront->Set(x);
		rightBack->Set(x);
	}
	//Components
	Spark *climbTake = new Spark(5);
	Spark *shooterBall = new Spark(6);
	Victor *constIntake = new Victor(0);

	Timer autoTimer;
	RobotDrive myRobot { leftFront , leftBack , rightBack , rightFront }; // Robot drive system
	Joystick playerOne { 0 }; // Only joystick
	Joystick playerTwo { 1 };
	bool shooterButton = playerOne.GetRawButton(1);
	bool climberButton = playerOne.GetRawButton(2);
	SmartDashboard* dash = new SmartDashboard();
	LiveWindow* lw = frc::LiveWindow::GetInstance();
	Timer timer;

	double speedLimiter = 1.25; //the number that the joystick input will be divided by to equal drive power
	double climberPower = 0;
	double intakePower = 0;

	void AutonomousInit() override {
		timer.Reset();
		timer.Start();
		gyro.Calibrate();

	}

	void AutonomousPeriodic() override {

		//SET THIS VARIABLE BEFORE RUN
		int areaVar = 4;
		//**^^^^^^^^**//
		if (areaVar == 1) {
			areaOne();
		}
		if (areaVar == 2) {
			areaTwo();
		}
		if (areaVar == 3) {
			areaThree();
		}
		if(areaVar==4){
			areaFour();
		}
	}
	void areaOne() {
		autoTimer.Reset();		//These two lines should begin every autonomous section
		autoTimer.Start();		//when using a timer.
//forwards until time in seconds
		myRobot.TankDrive (0.80 , 0 );
		if ( autoTimer.Get() >= 1.0 ){   //**   MEASURE THEN REPLACE    **//
			myRobot.StopMotor();
			autoTimer.Stop();
			autoTimer.Reset();
		}
	}
	void areaTwo(){
		autoTimer.Reset();
		autoTimer.Start();
		//forwards until time in seconds
		myRobot.TankDrive(0.5, 0.5 );
		if ( autoTimer.Get() >= 1.0 ) {
			myRobot.StopMotor();
			autoTimer.Stop();
			autoTimer.Reset();
		}	}
	void areaThree() {
		autoTimer.Reset();
		autoTimer.Start();
		//forwards until time in seconds
		myRobot.TankDrive(0.6, 0.5 );
		if ( autoTimer.Get() >= 3.0 ) {
			myRobot.StopMotor();
			autoTimer.Stop();
			autoTimer.Reset();
		}
		//Turn until time in seconds
		leftSide(0.5);
		rightSide(-0.5);
		if ( autoTimer.Get() >= 2.0 ) {
			myRobot.StopMotor();
			autoTimer.Stop();
			autoTimer.Reset();
		}}
		void areaFour(){
				autoTimer.Reset();
				autoTimer.Start();

				double angle = gyro.GetAngle(); //gets heading of gyro
				dash->PutNumber("Gyro Value:", angle);
				std::cout << angle;
				//forwards until time in seconds
				myRobot.Drive(0.25, angle*5);

				Wait(.05);


		}

	//OPERATOR CONTROL
	void TeleopInit() override {
		gyro.Reset();
	}

	void toggleSpeed(){ //sets speed limiter to toggle between dividing by 1 and 1.25
		if (speedLimiter == 1.25){
			speedLimiter = 1;
		}
		else{
			speedLimiter = 1.25;

		}
	}

	void toggleClimber(){ //this will increase the speed by 0.1 until it reaches 0.3 - then it will reset to 0
		if (climberPower > 0.2){
			climberPower = 0;
		}
		else{
			climberPower += 0.1;
		}
		climbTake->Set(climberPower);
	}

	void toggleIntake(){
		if (intakePower > 0.15){
			intakePower = 0;
		}
		else{
			intakePower += 0.06;
		}
		constIntake->Set(intakePower);
	}

	void TeleopPeriodic() override { //All referenced functions-"thisFunction()"-are directly above ^^

		if (playerTwo.GetRawButton(3)){
			toggleIntake();
		}
		if (playerTwo.GetRawButton(5)){
			toggleClimber();
		}
		if (playerOne.GetRawButton(6)){
			toggleSpeed();
		}

		//** Tank Drive with Controller #1 (The negative signs are for inversion)**//

		myRobot.TankDrive( -playerOne.GetRawAxis(1)/speedLimiter,-playerOne.GetRawAxis(5)/speedLimiter);

		//Controls Speed of Intake
		shooterBall->Set(playerTwo.GetRawAxis(3)/1.15);

		/*	if (playerTwo.GetRawButton(10)){ //change led color to red
					ledred->Set(true);
					ledblue->Set(false);
				}


*/
	}

	void TestPeriodic() override {
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot)
