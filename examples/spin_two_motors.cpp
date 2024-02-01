#include <Arduino.h>
#include <esp_task_wdt.h>

#include "motorgo_mini.h"

MotorGo::MotorGoMini* motorgo_mini;
MotorGo::MotorParameters motor_params_ch0;
MotorGo::MotorParameters motor_params_ch1;

MotorGo::PIDParameters velocity_pid_params_ch0;
MotorGo::PIDParameters velocity_pid_params_ch1;

// MotorGo loop task config
TaskHandle_t loop_foc_task;
TickType_t xLastWakeTime;
void loop_foc(void* pvParameters);

// Function to print at a maximum frequency
void freq_println(String str, int freq)
{
  static unsigned long last_print_time = 0;
  unsigned long now = millis();

  if (now - last_print_time > 1000 / freq)
  {
    Serial.println(str);
    last_print_time = now;
  }
}

void setup()
{
  Serial.begin(115200);

  delay(3000);

  // Setup motor parameters
  motor_params_ch0.pole_pairs = 11;
  motor_params_ch0.power_supply_voltage = 8.0;
  motor_params_ch0.voltage_limit = 8.0;
  motor_params_ch0.current_limit = 300;
  motor_params_ch0.velocity_limit = 100.0;
  motor_params_ch0.calibration_voltage = 2.0;
  motor_params_ch0.reversed = false;

  motor_params_ch1.pole_pairs = 11;
  motor_params_ch1.power_supply_voltage = 8.0;
  motor_params_ch1.voltage_limit = 8.0;
  motor_params_ch1.current_limit = 300;
  motor_params_ch1.velocity_limit = 100.0;
  motor_params_ch1.calibration_voltage = 2.0;
  motor_params_ch1.reversed = true;

  // Instantiate motorgo mini board
  motorgo_mini = new MotorGo::MotorGoMini();

  // Setup Ch0
  bool calibrate = false;
  motorgo_mini->init_ch0(motor_params_ch0, false);
  //   motorgo_mini->init_ch1(motor_params_ch1, calibrate);

  // Set velocity controller parameters
  // Setup PID parameters
  velocity_pid_params_ch0.p = 1.6;
  velocity_pid_params_ch0.i = 0.01;
  velocity_pid_params_ch0.d = 0.0;
  velocity_pid_params_ch0.output_ramp = 10000.0;
  velocity_pid_params_ch0.lpf_time_constant = 0.11;

  velocity_pid_params_ch1.p = 1.6;
  velocity_pid_params_ch1.i = 0.01;
  velocity_pid_params_ch1.d = 0.0;
  velocity_pid_params_ch1.output_ramp = 10000.0;
  velocity_pid_params_ch1.lpf_time_constant = 0.11;

  //   motorgo_mini->set_velocity_controller_ch0(velocity_pid_params_ch0);
  //   motorgo_mini->set_velocity_controller_ch1(velocity_pid_params_ch1);

  //   //   Set closed-loop velocity mode
  motorgo_mini->set_control_mode_ch0(MotorGo::ControlMode::Voltage);
  //   motorgo_mini->set_control_mode_ch1(MotorGo::ControlMode::Velocity);

  xTaskCreatePinnedToCore(
      loop_foc,       /* Task function. */
      "Loop FOC",     /* name of task. */
      10000,          /* Stack size of task */
      NULL,           /* parameter of the task */
      1,              /* priority of the task */
      &loop_foc_task, /* Task handle to keep track of created task */
      1);             /* pin task to core 1 */

  motorgo_mini->enable_ch0();
  //   motorgo_mini->enable_ch1();
}

void loop_foc(void* pvParameters)
{
  Serial.print("Loop FOC running on core ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    motorgo_mini->loop_ch0();
    // motorgo->loop_ch1();

    //   Print loop frequency
    esp_task_wdt_reset();
  }
}

void loop()
{
  vTaskDelay(10);
  // Run Ch0
  // motorgo_mini->loop_ch1();

  //   motorgo_mini->set_target_voltage_ch0(0);

  //   motorgo_mini->set_target_velocity_ch1(10.0);

  //   String str = "Velocity - Ch0: " +
  //   String(motorgo_mini->get_ch0_velocity()) +
  //                " Ch1: " + String(motorgo_mini->get_ch1_velocity());

  //   freq_println(str, 10);
}
