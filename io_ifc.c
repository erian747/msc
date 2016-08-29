#include <string.h>
#include "jansson.h"
#include "ch.h"

static int io_board_power = 0;
static int io_board_set_temperature = 67;
static int io_board_act_temperature = 0;
static int io_board_mode = 0;
static int io_board_pump_state = 0;

// Incoming JSON data from IO board
extern void incomming_io_board(const char *str);

static void io_ifc_build_message(void)
{
  json_t *root = json_object();
  json_t *modeString = json_string(io_board_mode ? "Auto" : "Manual");
  json_object_set_new(root, "Mode", modeString);
  char tmpStr[16];

  sprintf(tmpStr, "%d%c",io_board_mode ? io_board_set_temperature : io_board_power, io_board_mode ? 'C' : '%');

  json_object_set_new(root, "Ref", json_string(tmpStr));
  json_object_set_new(root, "PumpTemp", json_integer(io_board_act_temperature));
  json_object_set_new(root, "PumpState", json_string(io_board_pump_state ? "On" : "Off"));

  char *jdump = json_dumps(root, 0);
  // Send JSON data to UI
  if(jdump != 0)
  {
    incomming_io_board(jdump);

    //Release buffer
    chHeapFree(jdump);

  }

  json_decref(root);
}


void io_ifc_in(const char *str)
{
  json_error_t error;
  json_t *root = json_loads(str, 0, &error);
  if(root == 0)
  {
    //gwinPrintf(ghConsole, "json_loads error %s\n", error->text);
    return;
  }
  const char *key;
  json_t *value;

  json_object_foreach(root, key, value)
  {
    //gwinPrintf(ghConsole, "%s\n", key);
    if(strcmp(key, "Command") == 0)
    {
      const char *command = json_string_value(value);
      if(strcmp(command, "Up") == 0)
      {
        if(io_board_mode)
          io_board_set_temperature += (io_board_set_temperature < 100);
        else
          io_board_power += io_board_power < 100;
      }
      else if(strcmp(command, "Down") == 0)
      {
        if(io_board_mode)
          io_board_set_temperature -= (io_board_set_temperature > 0);
        else
          io_board_power -= (io_board_power > 0);
      }
      else if(strcmp(command, "Mode") == 0)
      {
        io_board_mode = !io_board_mode;
      }
      else if(strcmp(command, "PumpCtrl") == 0)
      {
        io_board_pump_state = !io_board_pump_state;
      }
    }

  }
  json_decref(root);
  // Reply to UI
  io_ifc_build_message();


}

uint8_t io_ifc_mode(void)
{
  return io_board_mode;
}


void io_ifc_sensor_data(float temp)
{
  io_board_act_temperature = temp * 10;
}

float io_ifc_ref(void)
{
  return io_board_mode ? io_board_set_temperature : (float)io_board_power/100;
}

void io_ifc_init(void)
{

}
