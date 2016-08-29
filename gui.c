#include <stdio.h>
#include <string.h>
#include "gfx.h"

static void mysave(uint16_t instance, const uint8_t *calbuf, size_t size)
{
  (void)instance;
}
static const uint8_t calibData[24] =
{

  0xCC,0x70,0xA9,0xBD,0x2A,0x40,0x53,0x3A,
  0xBC,0x6D,0xA4,0x43,0x7C,0xA5,0xC1,0xB9,
  0x90,0xFE,0x84,0x3D,0xD6,0xA3,0x4E,0xC1
};
bool_t LoadMouseCalibration(unsigned instance, void *data, size_t sz)
{
  memcpy(data, calibData, sz);
  return 1;
}
/*
static const char *myload(uint16_t instance)
{
  return (const char *)calibData;
}
*/
// Pages
static GHandle ghPageHome, ghPageConfig;

static GListener gl;
static GHandle   ghButton1, ghNextButton;
static GHandle ghLabel1, ghLabel2;
static GHandle graph;

static const GGraphStyle GraphStyle =
{
  { GGRAPH_POINT_DOT, 0, Blue },			// point
  { GGRAPH_LINE_SOLID, 2, Gray },			// line
  { GGRAPH_LINE_SOLID, 0, White },		// x axis
  { GGRAPH_LINE_SOLID, 0, White },		// y axis
  { GGRAPH_LINE_DASH, 5, Gray, 50 },		// x grid
  { GGRAPH_LINE_DOT, 7, Yellow, 50 },		// y grid
  GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS	// flags
};

static GHandle currentView = 0;


static void createWidgets(void)
{

  GWidgetInit wi;
  // Apply some default values for GWIN
  gwinWidgetClearInit(&wi);

  wi.g.width = gdispGetWidth();
  wi.g.height = gdispGetHeight();
  wi.g.x = 0;
  wi.g.y = 0;

  ghPageConfig		= gwinContainerCreate(0, &wi, 0);
  wi.g.show = TRUE;
  ghPageHome			= gwinContainerCreate(0, &wi, 0);

  // Apply the button parameters
  wi.g.width = 100;
  wi.g.height = 30;
  wi.g.y = 10;
  wi.g.x = 200;
  wi.text = "Next";

  // Create the actual button
  ghButton1 = gwinButtonCreate(0, &wi);


  wi.g.parent = ghPageHome;

  // Create the Temperature label
  wi.g.width = 180;
  wi.g.height = 20;
  wi.g.x = 10, wi.g.y = 10;
  wi.text = "N/A";
  ghLabel1 = gwinLabelCreate(0, &wi);
  gwinLabelSetAttribute(ghLabel1, 100, "Temp:");

  // Create the Setpoint label
  wi.g.width = 180;
  wi.g.height = 20;
  wi.g.x = 10, wi.g.y = 30;
  wi.text = "65.0";
  ghLabel2 = gwinLabelCreate(0, &wi);
  gwinLabelSetAttribute(ghLabel2, 100, "SetPoint:");

  wi.g.width = 300;
  wi.g.height = 180;
  wi.g.x = 10, wi.g.y = 60;
  graph = gwinGraphCreate(0, &wi.g);

  gwinGraphSetOrigin(graph, wi.g.width/2, wi.g.height/2);
  gwinGraphSetStyle(graph, &GraphStyle);
  gwinGraphDrawAxis(graph);



  wi.g.parent = ghPageConfig;

    // Create the labels
  wi.g.width = 180;
  wi.g.height = 20;
  wi.g.x = 10;
  wi.text = "0.0";
  GHandle label;
  for(int i = 0; i < 8; i++)
  {

    wi.g.y = 10+(wi.g.height+5)*i;
    label = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(label, 100, "Value:");
  }


}

typedef enum
{
  STATE_MASH,
  STATE_HEATUP,
  STATE_BOIL,
} brew_state_t;

static brew_state_t state;

extern int gui_test_main(void);
static msg_t guiThread(void *arg)
{

  gui_test_main();
/*
  // Get an Event
  while(TRUE)
  {

    GEvent* pe= geventEventWait(&gl, TIME_INFINITE);
    if(pe != NULL)
    {

      switch(pe->type)
      {
      case GEVENT_GWIN_BUTTON:
        if (((GEventGWinButton*)pe)->button == ghButton1)
        {
          if(currentView == ghPageHome)
          {
            currentView = ghPageConfig;
            gwinHide(ghPageHome);
            gwinShow(ghPageConfig);
          }
          else
          {
            currentView = ghPageHome;
            gwinHide(ghPageConfig);
            gwinShow(ghPageHome);
            gwinGraphDrawAxis(graph);
          }
          // Our button has been pressed
          if(state == STATE_MASH || state == STATE_BOIL)
          {
            gwinSetText(ghLabel2, "1800W",TRUE);
            state = STATE_HEATUP;
            //heater.setValue(1.00);
          }
          else if(state == STATE_HEATUP)
          {
            gwinSetText(ghLabel2, "900W",TRUE);
            //  heater.setValue(0.50);
            state = STATE_BOIL;
          }
        }
        break;

      default:
        break;
      }

    }
  }
  */
  return 0;
}


void gui_init(void)
{


  //gdispSetOrientation(GDISP_ROTATE_0);

  // Set the widget defaults
 // gwinSetDefaultFont(gdispOpenFont("DejaVuSans16"));
  //gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
 // gdispClear(Black);


  //ginputSetMouseCalibrationRoutines(0, mysave, myload, FALSE);
  // Attach the mouse input
  //gwinAttachMouse(0);

/*
  // create the widget
  createWidgets();
  currentView = ghPageHome;
  // We want to listen for widget events
  geventListenerInit(&gl);
  gwinAttachListener(&gl);
*/
  chThdCreateFromHeap(NULL, THD_WA_SIZE(8192), NORMALPRIO, guiThread, NULL);
}





