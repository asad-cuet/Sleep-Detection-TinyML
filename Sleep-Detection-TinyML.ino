#include <Arduino_BMI270_BMM150.h>

int degreesZ = 0;
float enmo=0;

void readZFromAccelerometer() 
{
    float x, y, z;
    if (IMU.accelerationAvailable()) 
    {
      IMU.readAcceleration(x, y, z);
    }

    enmo = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)) - 1.0;


    if(z > 0.01)
    {
      z = 100*z;
      degreesZ = map(z, 0, 97, 0, 90);
      Serial.print("anglez: ");
      Serial.print(degreesZ);
      Serial.print(", ");
      Serial.print("enmo: ");
      Serial.println(enmo);
    }
    
    if(z < -0.01)
    {
      z = 100*z;
      degreesZ = map(z, 0, -100, 0, -90);
      Serial.print("anglez: ");
      Serial.print(degreesZ);
      Serial.print(", ");
      Serial.print("enmo: ");
      Serial.println(enmo);
    }
      
}

#include <TensorFlowLite.h>

#include "main_functions.h"

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "constants.h"
#include "model.h"
#include "output_handler.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

constexpr int kTensorArenaSize = 2 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;


  model = tflite::GetModel(g_sine_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }


  static tflite::AllOpsResolver resolver;

  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  inference_count = 0;
}

void loop() {
  delay(10);

  readZFromAccelerometer();
  float x_val = degreesZ;
  float x_val2=enmo;
  // Place our calculated x value in the model's input tensor
  input->data.f[0] = x_val;
  input->data.f[1] = x_val2;

  // Run inference, and report any error
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed on x_val: %f\n",
                           static_cast<double>(x_val));
    return;
  }

  float y_val = output->data.f[0];
  float y_val2 = output->data.f[1];


  HandleOutput(error_reporter, x_val,x_val2, y_val,y_val2);


  inference_count += 1;
  if (inference_count >= kInferencesPerCycle) inference_count = 0;
}
