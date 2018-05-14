#pragma config(Sensor, in1,    Gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  Drive_Encode,   sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  Mogo_In_Switch1, sensorTouch)
#pragma config(Sensor, dgtl6,  Mogo_In_Switch2, sensorTouch)
#pragma config(Sensor, dgtl7,  Mogo_Out_Switch1, sensorTouch)
#pragma config(Sensor, dgtl8,  Mogo_Out_Switch2, sensorTouch)
#pragma config(Sensor, dgtl9,  ChainBar_In_Switch, sensorTouch)
#pragma config(Motor,  port1,           Mogo_1,        tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           Chain_Bar,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           R_B_Drive,     tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           R_M_Drive,     tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           R_F_Drive,     tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           L_B_Drive,     tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           L_M_Drive,     tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           L_F_Drive,     tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port9,           Intake,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          Mogo_2,        tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Durati6on Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(45)
#pragma userControlDuration(75)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamical properties of robot

const float Base_Robot_Mass = 5.45; // In Kg
const float Mass_Conversion_Factor = .00043;
float Robot_Mass = Base_Robot_Mass*Mass_Conversion_Factor;
const float Base_Mobile_Goal_Mass = 1.7;
const float Mobile_Goal_Mass = Base_Mobile_Goal_Mass*Mass_Conversion_Factor; // Kg

const float Base_Robot_MOI = .1366; // Kg-M^2
const float MOI_Conversion_Factor = .05;
float Robot_MOI = Base_Robot_MOI*MOI_Conversion_Factor;// + .08+0.06;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// prototypes
task Tank_Drive();
	const int Drive_Threshold = 8;

task Joystick_Velocity_Control();
	const int Joystick_Change_Threshold = 15;
	int L_Joystick;
	int R_Joystick;

task Tele_Mogo();
	int Mogo_Action_Number = 0;		// 1 if last mogo action brought mogo intake in, -1 if last mogo action brought mogo out (Used to determine passive power)
	int Mogo_In = 0;		// is 1 if either of the in swithces are pressed, 0 otherwise
	int Mogo_Out = 0;		// is 1 if either of the out switches are pressed, 0 otherwise

task Tele_Chain_Bar();

task Tele_Intake();
	short Tele_Intake_Passive_Power = 20;

void Drive(int Power, int Target_Value);
	int Drive_Enable = 0;

task Auto_Drive();
	int Drive_Power;
	int Drive_Target = 0;
	int Drive_Target_Sign;
	const int Drive_Refresh_Time = 20; // in miliseconds
	const float P_Drive = 1.25; 	// needs to be large enough to get us to the target from any displacement
	const float D_Drive = sqrt(4*P_Drive*Robot_Mass); // This is determined by solving 2nd order diff eq for critical damping. See notebook
	const int Drive_Power_Minimum = 12;
	const int Drive_BreakOut_Counter_Limit = ceil(100/Drive_Refresh_Time); 		// corresponds to ~.10 seconds
	const float Drive_Breakout_D = 10;
	int Drive_Offset = 0;

task Drive_Assist();
	int Drive_Assist_Enable = 0;
	int L_Drive_Correction = 0;
	int R_Drive_Correction = 0;
	const int Drive_Assist_Refresh_Time = 20; // in miliseconds
	const float P_Drive_Assist = 3.5;
	const float D_Drive_Assist = sqrt(4*P_Drive_Assist*Robot_MOI);

void Initialize_Gyro(void);

void Turn(int Power, int Angle);
	int Turn_Enable = 0;

task Auto_Turn();
	const float Gyro_Scale = 1.; //(360.0/368.3);
	int Turn_Power;
	int Turn_Target = 0;
	float Angle_Offset = 0; 					// This keeps track of how far off from straight we are after each action (turn)
	const int Turn_Refresh_Time = 20; // in miliseconds
	const float P_Turn = 5.00; 	// needs to be large enough to get us to the target from any displacement
	const float D_Turn = sqrt(4*P_Turn*Robot_MOI); // This is determined by soling 2nd order diff eq for critical damping. See notebook
	const int Turn_Power_Minimum = 8;
	const int Turn_BreakOut_Counter_Limit = ceil(100/Turn_Refresh_Time); 		// corresponds to ~.10 seconds
	const float Turn_BreakOut_D = 10;

void Mogo(int Power);
	int Mogo_Enable = 0;

task Auto_Mogo();
	int Mogo_Power = 0;
	const int Mogo_Refresh_Time = 25;

task LCD();
	int timer;

const char Auto_Power_Array[128] = {
	0, 15, 15, 15, 15, 15, 15, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 17,
	17, 17, 17, 17, 17, 18, 18, 18, 18, 18,
	18, 19, 19, 19, 19, 20, 20, 20, 20, 20,
	21, 21, 21, 21, 22, 22, 22, 22, 23, 23,
	23, 24, 24, 24, 24, 25, 25, 26, 26, 26,
	26, 27, 27, 27, 28, 28, 28, 28, 29, 29,
	29, 30, 30, 31, 31, 31, 32, 32, 33, 33,
	34, 34, 35, 35, 35, 36, 37, 37, 38, 38,
	39, 40, 41, 42, 42, 43, 43, 44, 45, 46,
	47, 48, 49, 51, 52, 52, 54, 56, 56, 58,
	59, 61, 62, 65, 67, 70, 73, 75, 77, 79,
	84, 84, 85, 86, 86, 87, 88, 89
};

const char Tele_Power_Array[128] = {
	0, 0, 0, 30, 30, 30, 30, 31, 31, 31,
	31, 31, 31, 31, 31, 31, 31, 31, 31, 32,
	32, 32, 32, 32, 32, 33, 33, 33, 33, 33,
	33, 34, 34, 34, 34, 35, 35, 35, 35, 35,
	36, 36, 36, 36, 37, 37, 37, 37, 38, 38,
	38, 39, 39, 39, 39, 40, 40, 41, 41, 41,
	41, 42, 42, 42, 43, 43, 43, 43, 44, 44,
	44, 45, 45, 46, 46, 46, 47, 47, 48, 48,
	49, 49, 50, 50, 50, 51, 52, 52, 53, 53,
	54, 55, 56, 57, 57, 58, 58, 59, 60, 61,
	62, 63, 64, 66, 67, 67, 69, 71, 71, 73,
	74, 76, 77, 80, 82, 85, 88, 90, 92, 94,
	95, 95, 95, 95, 95, 95, 95, 95
};

void Tele_Set_Drive(int L_Drive_Power, int R_Drive_Power);
	int L_Drive_Sign;
	int R_Drive_Sign;
	int L_Drive_Speed;
	int R_Drive_Speed;
void Auto_Set_Drive(int L_Drive_Power, int R_Drive_Power);

void Lock_Drive();

void Set_Mogo(int Power);

void Set_Chain_Bar(int Power);

void Set_Intake(int Power);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Driver-Control tasks and functions

task Tank_Drive() {
	// Declare local variables
	int L_Drive_Power;
	int R_Drive_Power;

	while(true) {
		// Update drive powers
		L_Drive_Power = (abs(vexRT[Ch3]) > Drive_Threshold) ? L_Joystick : 0;
		R_Drive_Power = (abs(vexRT[Ch2]) > Drive_Threshold) ? R_Joystick : 0;

		// Set drive
		Tele_Set_Drive(L_Drive_Power, R_Drive_Power);

		// Delay and loop
		wait1Msec(10);
		continue;
	} // while (true)
} // task Drive_Control()


task Joystick_Velocity_Control() {
	////////////////////////////////////////////////////////////////////////
	// Set up variables
	int Current_L_Joystick;
	int Old_L_Joystick;
	int Change_In_L_Joystick;
	int Sign_Change_In_L_Joystick;

	int Current_R_Joystick;
	int Old_R_Joystick;
	int Change_In_R_Joystick;
	int Sign_Change_In_R_Joystick;


	////////////////////////////////////////////////////////////////////////
	// Assign Old/Current values
	Current_L_Joystick = vexRT[Ch3];
	Old_L_Joystick = L_Joystick;

	Current_R_Joystick = vexRT[Ch2];
	Old_R_Joystick = R_Joystick;

	while(true) {

		////////////////////////////////////////////////////////////////////////
		// Update current values;
		Current_L_Joystick = vexRT[Ch3];
		Current_R_Joystick = vexRT[Ch2];

		////////////////////////////////////////////////////////////////////////
		// Update change values
		Change_In_L_Joystick = Current_L_Joystick - Old_L_Joystick;
		Sign_Change_In_L_Joystick = (Change_In_L_Joystick > 0) - (Change_In_L_Joystick < 0);

		Change_In_R_Joystick = Current_R_Joystick - Old_R_Joystick;
		Sign_Change_In_R_Joystick = (Change_In_R_Joystick > 0) - (Change_In_R_Joystick < 0);


		////////////////////////////////////////////////////////////////////////
		// Compare to previous value, change L/R joystick values if we're changing too quickly
		if (abs(Change_In_L_Joystick) > Joystick_Change_Threshold) {
			Current_L_Joystick = Old_L_Joystick + Sign_Change_In_L_Joystick*Joystick_Change_Threshold;
		}
		else {
			Current_L_Joystick = Current_L_Joystick;
		} // if (abs(L_Joystick_Change) > Joystick_Change_Threshold)

		if (abs(Change_In_R_Joystick) > Joystick_Change_Threshold) {
			Current_R_Joystick = Old_R_Joystick + Sign_Change_In_R_Joystick*Joystick_Change_Threshold;
		}
		else {
			Current_R_Joystick = Current_R_Joystick;
		} // if (abs(R_Joystick_Change) > Joystick_Change_Threshold)


		////////////////////////////////////////////////////////////////////////
		// Set global joystick values, loop
		L_Joystick = Current_L_Joystick;
		R_Joystick = Current_R_Joystick;

		////////////////////////////////////////////////////////////////////////
		// Update Old joystick values, wait, loop
		Old_L_Joystick = Current_L_Joystick;
		Old_R_Joystick = Current_R_Joystick;

		wait1Msec(20);
		continue;
	} // while (true)
} // task Joystick_Velocity_Control()


task Tele_Mogo(){
	while(true) {
		// Update Mogo_In, Mogo_Out
		Mogo_In = SensorValue[Mogo_In_Switch1] || SensorValue[Mogo_In_Switch2];
		Mogo_Out = SensorValue[Mogo_Out_Switch1] || SensorValue[Mogo_Out_Switch2];

		if(vexRT[Btn6U] == 1 && vexRT[Btn6D] == 0 && Mogo_In == 0) {
			Set_Mogo(100);
			Mogo_Action_Number = 1;
		} // 	if(vexRT[Btn6U] == 1 && vexRT[Btn6D] == 0 && Mogo_In == 0) {
		else if(vexRT[Btn6U] == 0 && vexRT[Btn6D] == 1 && Mogo_Out == 0) {
			Set_Mogo(-100);
			Mogo_Action_Number = -1;
		} // 	else if(vexRT[Btn6U] == 0 && vexRT[Btn6D] == 1 && Mogo_Out == 0) {
		else {
			if(Mogo_Out == 1) {
				Set_Mogo(10);
			} // if(Mogo_Out == 1) {
			else if(Mogo_In == 1) {
				Set_Mogo(-10);
			} // else if(Mogo_In == 1) {
			else if(Mogo_Action_Number == 1) {
				Set_Mogo(10);
			} // else if(Mogo_Action_Number == 1) {
			else if(Mogo_Action_Number == -1) {
				Set_Mogo(-10);
			} // else if(Mogo_Action_Number == -1) {
			else {
			  Set_Mogo(0);
			} // else {
		} // 	else {
	} // while(true) {
} // task Tele_Mogo(){

task Tele_Chain_Bar() {
	int Chain_Bar_In;
	while(true) {
		// Update sensor values
	  Chain_Bar_In = SensorValue[ChainBar_In_Switch];

		if(vexRT[Btn8D] == 1) {
			Tele_Intake_Passive_Power = 90;		//  when the chainbar is pressed down (usually when picking up cones) run intake full power in
			Set_Chain_Bar(90);
		} // if vexRT[Btn8D] == 1) {
		else if(Chain_Bar_In == 0) {
			Tele_Intake_Passive_Power = 20;
			Set_Chain_Bar(-100);
		} // if(vexRT[Btn8U] == 1 && Chain_Bar_In == 0) {
		else {
			Tele_Intake_Passive_Power = 20;
			Set_Chain_Bar(-15);
		} // else {
	} // while(true) {
} // task Tele_Chain_Bar() {

task Tele_Intake(){
	while(true) {
		if(vexRT[Btn7U] == 1) {
			Set_Intake(100);
		}
		else if(vexRT[Btn7D] == 1){
			Set_Intake(-100);
		}
		else {
			Set_Intake(Tele_Intake_Passive_Power);
		}
	} // while(true) {
} // task Tele_Intake(){

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Autonomous straight driving


void Drive(int Power, int Target_Value) {
	// Check that power is a positive number
	if (Power < 0) { return; }

	// Set drive global variables
	Drive_Power = Power;
	Drive_Target = Target_Value;
	Drive_Target_Sign = (Drive_Target > 0) - (Drive_Target < 0);

	// Enable auto_Drive
	Drive_Enable = 1;

} // void Run_Drive(int Power, int Target_Value)

task Auto_Drive() {
	// Declare Local variables
	int Drive_Pos;
	int Drive_Delta, Drive_Delta_h, Drive_Delta_2h;
	int Drive_Delta_Sign;
	float Drive_Delta_Derivative;
	int L_Drive_Power, R_Drive_Power;
	int Drive_BreakOut_Counter = 0;

	// Loop continuously, waiting to be enabled
	while(true) {
		// If enabled, set up drive stuff
		if(Drive_Enable) {
			//// Drive setup (setup once enabled, but prior to robot moving) ////

			// Reset encoders
			SensorValue[Drive_Encode] = 0;

			// Turn on drive assist
			Drive_Assist_Enable = 1;

			// Reset breakout counter
			Drive_BreakOut_Counter = 0;

			// Reset drive error
			Drive_Delta = Drive_Target; // This is Drive_Delta before drive has moved. This way, initial rate of change is 0.
			Drive_Delta_h = Drive_Delta;
			Drive_Delta_2h = Drive_Delta_h;
			Drive_Delta_Derivative = 0;

			// Loop while drive is enabled, breakout counter is below limit.
			while((Drive_Enable) && (Drive_BreakOut_Counter < Drive_BreakOut_Counter_Limit)) {
				// Update L,R,Avg values
				Drive_Pos = SensorValue[Drive_Encode] - Drive_Offset;

				// Update Drive_Deltas
				Drive_Delta_2h = Drive_Delta_h;
				Drive_Delta_h = Drive_Delta;
				Drive_Delta = (Drive_Target - Drive_Pos); 				// Update Error. Normalize this number such that it's 100 when we're one reference error away from target.
				Drive_Delta_Sign = (Drive_Delta > 0) - (Drive_Delta < 0); 		// update sign of error

				// PD loop //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Proportional //---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//

				// Derivative //---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//
				// Here we use a 3 point endpoint approximation of the derivative. f'(x) ~= (1/2h)*(3f(x) - 4f(x-h) + f(x+2h))
				Drive_Delta_Derivative = (1000./(2.*(float)Drive_Refresh_Time))*(float)(3.*Drive_Delta - 4.*Drive_Delta_h + Drive_Delta_2h);

				// Check for drive stall, add to counter if we are. This only applies if we're close to the target
				if (abs(Drive_Delta) < abs(Drive_Target*.5) && abs(Drive_Delta_Derivative*D_Drive) < Drive_Breakout_D) {
					Drive_BreakOut_Counter += 1;
				}
				else { Drive_BreakOut_Counter = 0; }			// Otherwise, reset breakout counter

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				// Assign L, R Drive_Power
				L_Drive_Power = P_Drive*Drive_Delta + D_Drive*Drive_Delta_Derivative; // Assign L

				if (abs(L_Drive_Power) > abs(Drive_Power)) { L_Drive_Power = (Drive_Delta_Sign)*(Drive_Power); }	// Limit max velocity (without correction) to drive power

				R_Drive_Power = L_Drive_Power;				// Assign R

				// Print P,D correction terms to debug stream (if connected)... mostly for tuning
				writeDebugStreamLine("P: %i D: %i",P_Drive*Drive_Delta,D_Drive*Drive_Delta_Derivative);

				/* Now, add in corrections (they diminish as you approach the target)
				if (abs(Drive_Delta) < .5*Drive_PID_Threshold) {
					L_Drive_Power += L_Drive_Correction*(2*(float)abs(Drive_Delta)/(float)Drive_PID_Threshold);
					R_Drive_Power += R_Drive_Correction*(2*(float)abs(Drive_Delta)/(float)Drive_PID_Threshold);
				} // if (abs(Drive_Delta) < Drive_PID_Threshold) {

				else {	// Case for when we're far from target, use full correcton
					L_Drive_Power += L_Drive_Correction;
					R_Drive_Power += R_Drive_Correction;
				} // else {
				*/

				L_Drive_Power += L_Drive_Correction*((float)L_Drive_Power/(float)Drive_Power);
				R_Drive_Power += R_Drive_Correction*((float)R_Drive_Power/(float)Drive_Power);

				// Now, set drive
				if(abs((L_Drive_Power + R_Drive_Power)/2.) < Drive_Power_Minimum) {
					Lock_Drive();
				}
				else {
					Auto_Set_Drive(L_Drive_Power, R_Drive_Power);
				}

				wait1Msec(Drive_Refresh_Time);
			} // while (Drive_Enable)
			//// shut down, reset global target values ////
			Auto_Set_Drive(0,0);
			Drive_Offset = Drive_Delta;
			Drive_Power = 0;
			Drive_Target = 0;
			Drive_Target_Sign = 0;
			Drive_BreakOut_Counter = 0;
			Auto_Set_Drive(0,0);
			Drive_Assist_Enable = 0;
			Drive_Enable = 0;
		}	// if(Drive_Enable)
		else {
			wait1Msec(Drive_Refresh_Time);
		} // else {
	} // while (true)
} // task Auto_Drive()


// This task makes sure the drive goes straight
task Drive_Assist(){
	// Declare Local variables
	float Angle;
	float Angle_Delta, Angle_Delta_h, Angle_Delta_2h;
	float Angle_Delta_Derivative;

	// Loop continuously, waiting to be enabled
	while(true) {
		// If enabled, set up drive stuff
		if(Drive_Assist_Enable) {
			//// Drive setup (setup once enabled, but prior to robot moving) ////
			SensorValue[Gyro] = 0;

			// Set initial offsets
			//L_Drive_Delta = -1*Drive_Assist_Offset; 				// Uses angle error from last turn
			//R_Drive_Delta = Drive_Assist_Offset;

			// Set up Drive_Deltas/derivatives
			Angle_Delta_h = Angle_Delta;
			Angle_Delta_2h = Angle_Delta_h;
			Angle_Delta_Derivative = 0;

			// Loop while drive_assist is enabled, breakout counter is below limit.
			while(Drive_Assist_Enable) {
				// Update Drive position
				Angle = .1*Gyro_Scale*((float)SensorValue[Gyro]) + Angle_Offset;

				// Update Drive_Deltas
				Angle_Delta_2h = Angle_Delta_h;
				Angle_Delta_h = Angle_Delta;
				Angle_Delta = Angle;					// Since an angle of 0 is the goal

				// PD loop //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Proportional //---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//

				// Derivative //---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//
				// Here we use a 3 point endpoint approximation of the derivative. f'(x) ~= (1/2h)*(3f(x) - 4f(x-h) + f(x+2h))
				Angle_Delta_Derivative = (1000./(2.*(float)Drive_Assist_Refresh_Time))*(float)(3.*Angle_Delta - 4.*Angle_Delta_h + Angle_Delta_2h);

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				// Assign L, R correction
				L_Drive_Correction = P_Drive_Assist*Angle_Delta + D_Drive_Assist*Angle_Delta_Derivative; // Assign L Correction
				L_Drive_Correction = Drive_Target_Sign*L_Drive_Correction; // So that the error is correct if the robot drives backwards
				R_Drive_Correction = -L_Drive_Correction; // Assign R Correction

				wait1Msec(Drive_Assist_Refresh_Time);
			} // while (Drive_Assist_Enable)
			//// shut down, reset global target values ////
			Angle_Offset = Angle;
			L_Drive_Correction = 0;
			R_Drive_Correction = 0;
		}	// if(Drive_Enable)
		else {
			wait1Msec(Drive_Assist_Refresh_Time);
		} // else {
	} // while (true)
} // task Drive_Assist()


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Autonomous turning drive

// This function is used to start up the Gyro
void Initialize_Gyro(void) {
	SensorType[in1] = sensorNone; //in1 is Gyro1
	wait1Msec(1000);
	SensorType[in1] = sensorGyro;
	wait1Msec(2000);
}

void Turn(int Power, int Angle) {
	// Check that power is a positive number
	if (Power < 0) { return; }

	// Set drive global variables
	Turn_Power = Power;
	Turn_Target = Angle;

	// Enable auto_Drive
	Turn_Enable = 1;

} // void Turn(int Power, int Target_Value)


task Auto_Turn() {
	// Declare Local variables
	float Angle;
	float Angle_Delta, Angle_Delta_h, Angle_Delta_2h;
	float Angle_Delta_Derivative, Angle_Delta_Sign;
	int L_Drive_Power, R_Drive_Power;
	int Turn_BreakOut_Counter;

	// Loop continuously, waiting to be enabled
	while(true) {
		// If enabled, set up drive stuff
		if(Turn_Enable) {
			//// Turn setup (setup once enabled, but prior to robot moving) ////

			// Reset Gyro
			SensorValue[Gyro] = 0;			// Use last offset as starting angle
			Drive_Offset = 0; 					// Reset drive offset. We're changing directions, so the drive offset no longer applies/will not move us in the right direction.

			// Reset breakout counter
			Turn_BreakOut_Counter = 0;

			// Reset turn delta
			Angle_Delta = Turn_Target; // This is Drive_Delta before drive has moved. This way, initial rate of change is 0.
			Angle_Delta_h = Angle_Delta;
			Angle_Delta_2h = Angle_Delta_h;
			Angle_Delta_Derivative = 0;

			// Loop while turn is enabled, breakout counter is below limit.
			while((Turn_Enable) && (Turn_BreakOut_Counter < Turn_BreakOut_Counter_Limit)) {
				// Update Angle value
				Angle = .1*Gyro_Scale*((float)SensorValue[Gyro]) + Angle_Offset;     // Gyro outputs angles in 10ths of a degree... divide by 10 to convert to units of degrees

				// Update Turn_Deltas
				Angle_Delta_2h = Angle_Delta_h;
				Angle_Delta_h = Angle_Delta;
				Angle_Delta = (Turn_Target - Angle);
				Angle_Delta_Sign = (Angle_Delta > 0) - (Angle_Delta < 0); 		// update sign of error

				// PD loop //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Proportional //---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//

				// Derivative //---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//---//
				// Here we use a 3 point endpoint approximation of the derivative. f'(x) ~= (1/2h)*(3f(x) - 4f(x-h) + f(x+2h))
				Angle_Delta_Derivative = (1000./(2.*(float)Turn_Refresh_Time))*(float)(3.*Angle_Delta - 4.*Angle_Delta_h + Angle_Delta_2h);

				// Check for turn stall, add to counter if we are. This only applies if we're close to the target
				if (abs(Angle_Delta) < abs(Turn_Target*.5) && abs(Angle_Delta_Derivative) < D_Turn*Turn_BreakOut_D) {
					Turn_BreakOut_Counter += 1;
				}
				else { Turn_BreakOut_Counter = 0; }			// Otherwise, reset breakout counter

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				// Assign L, R Drive_Power
				R_Drive_Power = P_Turn*Angle_Delta + D_Turn*Angle_Delta_Derivative; // Assign R (For CCW rotation, Right side should go forward, L backwards)

				R_Drive_Power = (abs(R_Drive_Power) < Turn_Power_Minimum) ? 0 : R_Drive_Power;	// Set L to 0 if we're very close to 0 power (prevent oscillations

				if (abs(R_Drive_Power) > abs(Turn_Power)) { R_Drive_Power = (Angle_Delta_Sign)*(Turn_Power); }	// Limit max velocity (without correction) to drive power

				L_Drive_Power = -R_Drive_Power;				// Assign L

				writeDebugStreamLine("P: %i D: %i",P_Turn*Angle_Delta,D_Turn*Angle_Delta_Derivative);

				// Now, set drive
				Auto_Set_Drive(L_Drive_Power, R_Drive_Power);

				wait1Msec(Turn_Refresh_Time);
			} // while (Turn_Enable)
			//// shut down, reset global target values ////
			Angle_Offset = -Angle_Delta;
			Turn_Power = 0;
			Turn_Target = 0;
			Turn_BreakOut_Counter = 0;
			Auto_Set_Drive(0,0);
			Turn_Enable = 0;
		}	// if(Turn_Enable)
		else {
			wait1Msec(20);
		} // else {
	} // while (true)
} // task Auto_Turn()

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Autonomous Mobile goal control

void Mogo(int Power) {
	// Set drive global variables
	Mogo_Power = Power;

	// Enable auto_Drive
	Mogo_Enable = 1;
} // task Mogo(int Power, int Time) {

task Auto_Mogo(){
	// Set up local variables
	int Mogo_Power_Sign;
	int Mogo_In = 0;		// is 1 if either of the in swithces are pressed, 0 otherwise
	int Mogo_Out = 0;		// is 1 if either of the out switches are pressed, 0 otherwise

	while(true) {
		// Update Mogo_In and Mogo_Out (needed for passive power)
		Mogo_In = SensorValue[Mogo_In_Switch1] || SensorValue[Mogo_In_Switch2];
		Mogo_Out = SensorValue[Mogo_Out_Switch1] || SensorValue[Mogo_Out_Switch2];

		if(Mogo_Enable == 1) {
			//// Mogo setup (setup once enabled, but prior to robot moving) ////

			// Determine sign of power
			Mogo_Power_Sign = (Mogo_Power > 0) - (Mogo_Power < 0);

			// Set mogo to Power (get mogo moving)
			Set_Mogo(Mogo_Power);

			// Loop while mogo is enabled
			while(Mogo_Enable == 1) {
				// Update Mogo_In, Mogo_Out
				Mogo_In = SensorValue[Mogo_In_Switch1] || SensorValue[Mogo_In_Switch2];
				Mogo_Out = SensorValue[Mogo_Out_Switch1] || SensorValue[Mogo_Out_Switch2];

				if(Mogo_Power_Sign == 1 && Mogo_In == 1) {
					Set_Mogo(10);
					Mogo_Enable = 0;
					break;
				} // if(Mogo_Power_Sign == 1 && Mogo_In == 1) {
				else if(Mogo_Power_Sign == -1 && Mogo_Out == 1) {
					Set_Mogo(-10);
					Mogo_Enable = 0;
					break;
				} // else if(Mogo_Power_Sign == -1 && Mogo_Out == 1) {
				else { continue; }

				wait1Msec(Mogo_Refresh_Time);
			} // while (Mogo_Enable)

			//// shut down, reset global target values ////
			Mogo_Power = 0;
			Mogo_Enable = 0;
		}	// if(Mogo_Enable)
		else {
			if(Mogo_Out == 1) {
				Set_Mogo(10);
			} // if(Mogo_Out == 1) {
			else if(Mogo_In == 1) {
				Set_Mogo(-10);
			} // else if(Mogo_In == 1) {
			else {
				Set_Mogo(0);
			} // else {
			wait1Msec(Mogo_Refresh_Time);
		} // else {
	} // while (true)
} // task Tele_Mogo(){

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set-Drive


void Tele_Set_Drive(int L_Drive_Power, int R_Drive_Power) {
	// Find sign of inputs
	L_Drive_Sign = (L_Drive_Power > 0) - (L_Drive_Power < 0);
	R_Drive_Sign = (R_Drive_Power > 0) - (R_Drive_Power < 0);

	// Find magnitudes
	L_Drive_Speed = abs(L_Drive_Power);
	R_Drive_Speed = abs(R_Drive_Power);

	// Make sure magnitudes are less than 127
	L_Drive_Speed = (L_Drive_Speed > 127) ? 127 : L_Drive_Speed;
	R_Drive_Speed = (R_Drive_Speed > 127) ? 127 : R_Drive_Speed;

	motor[R_B_Drive] = R_Drive_Sign*Tele_Power_Array[R_Drive_Speed];
	motor[R_M_Drive] = R_Drive_Sign*Tele_Power_Array[R_Drive_Speed];
	motor[R_F_Drive] = R_Drive_Sign*Tele_Power_Array[R_Drive_Speed];
	motor[L_B_Drive] = L_Drive_Sign*Tele_Power_Array[L_Drive_Speed];
	motor[L_M_Drive] = L_Drive_Sign*Tele_Power_Array[L_Drive_Speed];
	motor[L_F_Drive] = L_Drive_Sign*Tele_Power_Array[L_Drive_Speed];
} // void Tele_Set_Drive(int Power)

void Auto_Set_Drive(int L_Drive_Power, int R_Drive_Power) {
	// Find sign of inputs
	L_Drive_Sign = (L_Drive_Power > 0) - (L_Drive_Power < 0);
	R_Drive_Sign = (R_Drive_Power > 0) - (R_Drive_Power < 0);

	// Find magnitudes
	L_Drive_Speed = abs(L_Drive_Power);
	R_Drive_Speed = abs(R_Drive_Power);

	// Make sure magnitudes are less than 127
	L_Drive_Speed = (L_Drive_Speed > 127) ? 127 : L_Drive_Speed;
	R_Drive_Speed = (R_Drive_Speed > 127) ? 127 : R_Drive_Speed;

	motor[R_B_Drive] = R_Drive_Sign*Auto_Power_Array[R_Drive_Speed];
	motor[R_M_Drive] = R_Drive_Sign*Auto_Power_Array[R_Drive_Speed];
	motor[R_F_Drive] = R_Drive_Sign*Auto_Power_Array[R_Drive_Speed];
	motor[L_B_Drive] = L_Drive_Sign*Auto_Power_Array[L_Drive_Speed];
	motor[L_M_Drive] = L_Drive_Sign*Auto_Power_Array[L_Drive_Speed];
	motor[L_F_Drive] = L_Drive_Sign*Auto_Power_Array[L_Drive_Speed];
} // void Auto_Set_Drive(int Power)


void Lock_Drive() {
	motor[R_B_Drive] = 7;
	motor[R_M_Drive] = -7;
	motor[R_F_Drive] = 7;
	motor[L_B_Drive] = 7;
	motor[L_M_Drive] = -7;
	motor[L_F_Drive] = 7;
} // void Lock_Drive(int Power)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set_Mogo

void Set_Mogo(int Power) {
	motor[Mogo_1] = Power;
	motor[Mogo_2] = Power;
} // void Set_Mogo(int Power) {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set_Chain_Bar

void Set_Chain_Bar(int Power) {
	motor[Chain_Bar] = Power;
} // void Set_Chain_Bar(int Power) {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set_Intake

void Set_Intake(int Power) {
	motor[Intake] = Power;
} // void Set_Intake(int Power) {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LCD

task LCD() {
	bLCDBacklight = true;
	// Define variables
	string Bat_Main;
	string Bat_Backup;
	string Gyroscope;
	string Encode;
	string L_Cor, R_Cor;
	string Offset;
	string Time;
	string DriveD;

	int Enc;
	float Gyro_Val;

	// Encoder + battery loop.
	while (true) {
		// Update variables
		Enc = SensorValue[Drive_Encode];
		Gyro_Val = .1*Gyro_Scale*((float)SensorValue[Gyro]);


		// Update strings
		sprintf(Bat_Main,"%1.2f",nImmediateBatteryLevel/1000.0,"V");
		sprintf(Bat_Backup,"%1.2f",BackupBatteryLevel/1000.0,"V");
		sprintf(Encode,"%4d",Enc);
		sprintf(Gyroscope,"%5.2f",Gyro_Val);
		sprintf(Time,"%4d",timer);
		sprintf(DriveD,"%4f",D_Drive);
		//sprintf(L_Cor,"%4.0f",L_Drive_Correction);
		//sprintf(R_Cor,"%4.0f",R_Drive_Correction);
		//sprintf(Offset,"%4.0f",Drive_Offset);

		// 1st line
		displayLCDString(0,0,"E:");
		displayNextLCDString(Encode);
		displayNextLCDString(" G:");
		displayNextLCDString(Gyroscope);

		// 2nd line
		displayLCDString(1,0,"M: ");
		displayNextLCDString(Bat_Main);
		displayNextLCDString(" D: ");
		displayNextLCDString(DriveD);

		wait1Msec(50);
	} // while (true)
} // task LCD()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-Auto, Auto, usercontrol
void pre_auton() {
	bStopTasksBetweenModes = true;
	Initialize_Gyro();
	SensorValue[Drive_Encode] = 0;
	SensorValue[Gyro] = 0;
}

task autonomous() {
	startTask(Auto_Drive);
	startTask(Drive_Assist);
	startTask(Auto_Turn);
	startTask(Auto_Mogo);
	startTask(LCD);


	// Drive test
	/*
	clearTimer(T1);
	Drive(110,500);
	while(Drive_Enable) {
		wait1Msec(20);
	} // while(Drive_Enable) {
	timer = time1(T1);
	wait1Msec(20000);
	*/

	/*
	// Turn test
	clearTimer(T1);
	Turn(110,90);
	while (Turn_Enable) {
		wait1Msec(25);
	} // while (Turn_Enable)
	timer = time1(T1);
	*/

	/*
	// Mogo Test
	Mogo(50,500);
	while(Mogo_Enable) {
		wait1Msec(50);
	} // while (Turn_Enable)

	Mogo(-50,500);
	while(Mogo_Enable) {
		wait1Msec(50);
	} // while (Turn_Enable)
	*/

	/*
	// Turn Drive (Error hand off) test
	Turn(100,-90);
	while(Turn_Enable) {
		wait1Msec(25);
	}

	wait1Msec(2500);

	Drive(100,1500);
	while(Drive_Enable) {
		wait1Msec(25);
	}
	*/

	// Programming skills
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pick up first two mobile goals

	// First, lift up the chainbar
	Set_Chain_Bar(-100);
	Set_Intake(40);
	while(SensorValue[ChainBar_In_Switch] == 0) {
		wait1Msec(20);
	}
	Set_Chain_Bar(-15);

	Angle_Offset = 0;
	Mogo(-100);
	Drive(100,500);
	while(Mogo_Enable || Drive_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable || Drive_Enable) {

	Drive(110,1800);
	while(Drive_Enable) {
		wait1Msec(25);
	} // 	while(Drive_Enable) {
	Robot_MOI += 2.*(Mobile_Goal_Mass*.15*.15); 	// to account for the fact that we're carrying mobile goals
	//0.15 m is the approximate/effective radius of the mobile goals for calculating moment of inertia
	Robot_Mass += 2*Mobile_Goal_Mass;

	Drive(85,950);
	Mogo(100);
	while(Mogo_Enable || Drive_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable || Drive_Enable) {

	// Drop off preload
	wait1Msec(500);
	Set_Intake(-100);
	wait1Msec(300);
	Set_Intake(0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Manuiver to first scoring zone
	Turn(120,-45);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	Drive(85,850);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(120,45);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Drop off first two mogo's
	Drive(127,650);
	Mogo(-100);
	while(Mogo_Enable || Drive_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable || Drive_Enable) {
	Angle_Offset = 0;
	//reset angle offset since the robot drives perpendicular to the bar whenever it drops off the mobile goals and effectively realigns the robot

	Set_Mogo(-30);
	wait1Msec(250);
	Robot_MOI -= Mobile_Goal_Mass*2*.15*.15; 	// We've dropped off mogo's
	Robot_Mass -= 2*Mobile_Goal_Mass;

	Drive(125,-740);
	//use this command again so in case the bars try to push the mogo intake up again, it will stay down
	while(Drive_Enable) {
		if(SensorValue[Mogo_Out_Switch1] == 0 || SensorValue[Mogo_Out_Switch2] == 0) { Set_Mogo(-50); }
		else{ Set_Mogo(-20); }

		wait1Msec(25);
	} // while(Mogo_Enable || Drive_Enable) {
	Set_Mogo(0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Move into position for second set of mobile goals

	Mogo(100);
	while(Mogo_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable) {

	Drive(120,250);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(120,-90);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	Drive(120,650);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(120,-90);
	while(Mogo_Enable || Turn_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable || Turn_Enable) {

	/*
	Drive(110,-200);
	Mogo(-100);
	while(Drive_Enable || Mogo_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable || Drive_Enable) {
	*/

	Auto_Set_Drive(-110,-110);
	wait1Msec(600);
	Auto_Set_Drive(0,0);

	Mogo(-100);
	while(Mogo_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable) {
	wait1Msec(500);
	Angle_Offset = 0; 	// Robot should have backed into bar, realigning. Errase any accumulated error
	Drive_Offset = 0;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get second set of mobile goals

	Drive(110,2050);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Robot_MOI += Mobile_Goal_Mass*2*.15*.15; 	// We've picked up two mogos
	Robot_Mass += 2*Mobile_Goal_Mass;

	Mogo(100);
	Drive(85,1200);
	while(Drive_Enable || Mogo_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable || Mogo_Enable) {

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Manuiver to second scoring zone

	Turn(120,-45);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	Drive(85,800);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(120,45);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Drop off second two mogo's

	Drive(127,650);
	Mogo(-100);
	while(Mogo_Enable || Drive_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable || Drive_Enable) {
	Angle_Offset = 0;

	wait1Msec(250);
	Robot_MOI -= Mobile_Goal_Mass*2*.15*.15; 	// We've dropped off mogo's
	Robot_Mass -= 2*Mobile_Goal_Mass;

	Drive(125,-740);
	//use this command again so in case the bars try to push the mogo intake up again, it will stay down
	while(Drive_Enable) {
		if(SensorValue[Mogo_Out_Switch1] == 0 || SensorValue[Mogo_Out_Switch2] == 0) { Set_Mogo(-50); }
		else{ Set_Mogo(-20); }

		wait1Msec(25);
	} // while(Mogo_Enable || Drive_Enable) {
	Set_Mogo(0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get in position for 5th mogo
	Mogo(100);
	while(Mogo_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable) {

	Drive(110,250);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(110,-90);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	Drive(110,-1200);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(110,-45);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	Auto_Set_Drive(-125,-125);
	wait1Msec(270);
	Auto_Set_Drive(0,0);

	Mogo(-100);
	while(Mogo_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable) {

	Angle_Offset = 0;	// We've backed up against the wall, all errors should be reset
	Drive_Offset = 0;

	Drive(120,200);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(120,-90);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get 5th mogo
	Drive(120,1050);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Mogo(100);
	while(Mogo_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable) {

	Robot_MOI += Mobile_Goal_Mass*.15*.15; 	// We've picked up a mogo
	Robot_Mass += Mobile_Goal_Mass;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Score 5th mogo
	Drive(110,-1300);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(110,210);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	Drive(120,500);
	while(Drive_Enable){
		wait1Msec(25);
	}

	Mogo(-100);
	while(Mogo_Enable) {
		wait1Msec(25);
	} // while(Mogo_Enable) {

	Robot_MOI -= Mobile_Goal_Mass*.15*.15; 	// We've dropped off a mogo
	Robot_Mass -= Mobile_Goal_Mass;

	Drive(120,-500);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {

	Turn(110,-30);
	while(Turn_Enable) {
		wait1Msec(25);
	} // while(Turn_Enable) {

	Drive(120,-1500);
	while(Drive_Enable) {
		wait1Msec(25);
	} // while(Drive_Enable) {
}

task usercontrol() {
	// Start tasks
	startTask(Joystick_Velocity_Control);		// Restricts acceleration of drive motors
	startTask(Tank_Drive);									// Controls tank drive
	startTask(Tele_Mogo);												// Controls mogo intake
	startTask(LCD);													// Updates LCD
	startTask(Tele_Chain_Bar);
	startTask(Tele_Intake);

	// Infinite loop to keep tele-op running
	while(true) {
		Drive_Enable = 0;
		Drive_Assist_Enable = 0;
		Turn_Enable  = 0;
		wait1Msec(50);
	}
} // task main()
