/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "gfx.h"
#include <string.h>
#include "jansson.h"
#include "serial.h"
#include "io_ifc.h"

/**
 * This demo demonstrates many of the GWIN widgets.
 * On the "Radio" tab try playing with the color radio buttons.
 * On the "Checkbox" tab try playing with the "Disable All" checkbox.
 */

/**
 * The image files must be stored on a GFILE file-system.
 * Use either GFILE_NEED_NATIVEFS or GFILE_NEED_ROMFS (or both).
 *
 * The ROMFS uses the file "romfs_files.h" to describe the set of files in the ROMFS.
 */

/* Our custom yellow style */
/*
static const GWidgetStyle YellowWidgetStyle = {
	Yellow,							// window background
  Green,
	// enabled color set
	{
		HTML2COLOR(0x0000FF),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xE0E0E0),		// progress - inactive area
	},

	// disabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0x808080),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xC0E0C0),		// progress - active area
	},

	// pressed color set
	{
		HTML2COLOR(0xFF00FF),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x808080),		// fill
		HTML2COLOR(0x00E000),		// progress - active area
	},
};
*/
/* The variables we need */
static font_t		font;
static GListener	gl;
static GHandle		ghConsole;
static GHandle		ghTabButtons, ghTabCheckboxes, ghTabLabels, ghTabRadios, ghTabImages; ;
static GHandle		ghPgControls, ghPgCheckboxes, ghPgLabels, ghPgRadios, ghPgImages;
static GHandle		ghModeButton, ghTempSetpointUpBtn, ghTempSetpointDownBtn, ghPumpOnOffBtn;
//static GHandle		ghSlider1, ghSlider2, ghSlider3, ghSlider4;
static GHandle		ghCheckbox1, ghCheckbox2, ghCheckDisableAll;
static GHandle		ghLabelSlider1, ghLabelSlider2, ghLabelSlider3, ghLabelSlider4, ghLabelRadio1;
static GHandle     modeLabel, heaterLabel;

static GHandle		ghRadio1, ghRadio2;
static GHandle		ghRadioBlack, ghRadioWhite, ghRadioYellow;
//static GHandle		ghList1, ghList2, ghList3, ghList4;
static GHandle		ghImage1;
//static GHandle		ghProgressbar1;
//static gdispImage	imgYesNo;


static GHandle pumpTempLabel;
static GHandle pumpTempValue;

/* Some useful macros */
#define	ScrWidth			gdispGetWidth()
#define	ScrHeight			gdispGetHeight()

#define BUTTON_PADDING		20
#define TAB_HEIGHT			30
#define LABEL_HEIGHT		15
#define BUTTON_WIDTH		50
#define BUTTON_HEIGHT		30
#define LIST_WIDTH			75
#define LIST_HEIGHT			80
#define SLIDER_WIDTH		20
#define CHECKBOX_WIDTH		80
#define CHECKBOX_HEIGHT		20
#define RADIO_WIDTH			50
#define RADIO_HEIGHT		20
#define COLOR_WIDTH			80
#define DISABLEALL_WIDTH	100
#define GROUP_TABS			0
#define GROUP_YESNO			1
#define GROUP_COLORS		2

// Wrap tabs onto the next line if they don't fit.
static void settabtext(GWidgetInit *pwi, char *txt) {
	if (pwi->g.x >= ScrWidth) {
		pwi->g.x = 0;
		pwi->g.y += pwi->g.height;
	}
	pwi->text = txt;
	pwi->g.width = gdispGetStringWidth(pwi->text, font) + BUTTON_PADDING;
	if (pwi->g.x + pwi->g.width > ScrWidth) {
		pwi->g.x = 0;
		pwi->g.y += pwi->g.height;
	}
}

// Wrap tabs onto the next line if they don't fit.
static void setbtntext(GWidgetInit *pwi, coord_t maxwidth, char *txt) {
	if (pwi->g.x >= maxwidth) {
		pwi->g.x = 5;
		pwi->g.y += pwi->g.height+1;
	}
	pwi->text = txt;
	pwi->g.width = gdispGetStringWidth(pwi->text, font) + BUTTON_PADDING;
	if (pwi->g.x + pwi->g.width > maxwidth) {
		pwi->g.x = 5;
		pwi->g.y += pwi->g.height+1;
	}
}

static const GWidgetStyle OnOffWidgetStyleOff = {
	HTML2COLOR(0xFFFFFF),			// window background
	HTML2COLOR(0xFFFFFF),			// window background

	// enabled color set
	{
		HTML2COLOR(0x000000),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0xC00000),		// fill
		HTML2COLOR(0xE0E0E0),		// progress - inactive area
	},

	// disabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0x808080),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xC0E0C0),		// progress - active area
	},

	// pressed color set
	{
		HTML2COLOR(0x404040),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0xFF0000),		// fill red
		HTML2COLOR(0x00E000),		// progress - active area
	},
};

static const GWidgetStyle OnOffWidgetStyleOn = {
	HTML2COLOR(0xFFFFFF),			// window background
	HTML2COLOR(0xFFFFFF),			// window background

	// enabled color set
	{
		HTML2COLOR(0x000000),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x00C000),		// fill
		HTML2COLOR(0xE0E0E0),		// progress - inactive area
	},

	// disabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0x808080),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xC0E0C0),		// progress - active area
	},

	// pressed color set
	{
		HTML2COLOR(0x404040),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x00FF00),		// fill greeb
		HTML2COLOR(0x00E000),		// progress - active area
	},
};


/**
 * Create all the widgets.
 * With the exception of the Pages they are all initially visible.
 *
 * This routine is complicated by the fact that we want a dynamic
 * layout so it looks good on small and large displays.
 * It is tested to work on 320x272 as a minimum LCD size.
 */
static void createWidgets(void) {
	GWidgetInit		wi;
	coord_t			border;
  font_t dv24Font = gdispOpenFont("DejaVuSans24");
  font_t dv32aaFont = gdispOpenFont("DejaVuSans32_aa");

	gwinWidgetClearInit(&wi);

	// Create the Tabs
	wi.g.show = TRUE; wi.customDraw = gwinRadioDraw_Tab;
	wi.g.height = TAB_HEIGHT; wi.g.y = 0;
	wi.g.x = 0; setbtntext(&wi, ScrWidth, "Control");
	ghTabButtons     = gwinRadioCreate(0, &wi, GROUP_TABS);
	//wi.g.x += wi.g.width; settabtext(&wi, "Sliders");
	//ghTabSliders     = gwinRadioCreate(0, &wi, GROUP_TABS);
	wi.g.x += wi.g.width; settabtext(&wi, "Settings");
	ghTabCheckboxes  = gwinRadioCreate(0, &wi, GROUP_TABS);
	wi.g.x += wi.g.width; settabtext(&wi, "Radios");
	ghTabRadios      = gwinRadioCreate(0, &wi, GROUP_TABS);
	//wi.g.x += wi.g.width; settabtext(&wi, "Lists");
	//ghTabLists       = gwinRadioCreate(0, &wi, GROUP_TABS);
	wi.g.x += wi.g.width; settabtext(&wi, "Monitor");
	ghTabLabels      = gwinRadioCreate(0, &wi, GROUP_TABS);
	wi.g.x += wi.g.width; settabtext(&wi, "Images");
	ghTabImages      = gwinRadioCreate(0, &wi, GROUP_TABS);
	//wi.g.x += wi.g.width; settabtext(&wi, "Progressbar");
	//ghTabProgressbar = gwinRadioCreate(0, &wi, GROUP_TABS);
	wi.g.y += wi.g.height;
	wi.customDraw = 0;

	// Calculate page borders based on screen size
	border = ScrWidth < 450 ? 1 : 5;

	// Create the Pages
	wi.g.show = FALSE;
	wi.g.x = border; wi.g.y += border;
	wi.g.width = ScrWidth/2 - border; wi.g.height = ScrHeight-wi.g.y-border;
	ghPgControls			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	//ghPgSliders			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	ghPgCheckboxes		= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	ghPgRadios			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	//ghPgLists			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	ghPgLabels			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

	wi.g.width = ScrWidth - border;
	ghPgImages			= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	//ghPgProgressbars	= gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	wi.g.show = TRUE;

	// Console - we apply some special colors before making it visible
	wi.g.x = ScrWidth/2+border;
	wi.g.width = ScrWidth/2 - 2*border;
	ghConsole = gwinConsoleCreate(0, &wi.g);
    gwinSetColor(ghConsole, Black);
    gwinSetBgColor(ghConsole, White);

    // Buttons
	wi.g.parent = ghPgControls;
	wi.g.x = 5;
  wi.g.y = 5;
  wi.g.height = 35;

  wi.text = "TP";
  pumpTempLabel = gwinLabelCreate(0, &wi);
  gwinSetFont(pumpTempLabel, dv32aaFont);
  wi.g.x += gdispGetStringWidth(wi.text, dv32aaFont)+5;
  wi.text = "NA";
  pumpTempValue = gwinLabelCreate(0, &wi);
  gwinSetFont(pumpTempValue, dv32aaFont);

	wi.g.width = BUTTON_WIDTH; wi.g.height = BUTTON_HEIGHT;
	wi.g.y = wi.g.height+10;
	wi.g.x = 5; //setbtntext(&wi, gwinGetInnerWidth(ghPgControls), "Mode");
	wi.text = "Mode";
	ghModeButton = gwinButtonCreate(0, &wi);
	wi.g.y += wi.g.height+3;
	wi.text = "Up";
	ghTempSetpointUpBtn = gwinButtonCreate(0, &wi);
	wi.g.y += wi.g.height+3;
	wi.text = "Down";
	ghTempSetpointDownBtn = gwinButtonCreate(0, &wi);
	wi.g.y += wi.g.height+3;
	wi.text = "Off";
	ghPumpOnOffBtn = gwinButtonCreate(0, &wi);
  gwinSetStyle(ghPumpOnOffBtn, &OnOffWidgetStyleOff);

  wi.g.x += BUTTON_WIDTH+3; wi.g.y = gwinGetScreenY(ghModeButton) - TAB_HEIGHT;
  wi.g.width = ScrWidth - (BUTTON_WIDTH + 5);

  wi.text = "NA";
  modeLabel = gwinLabelCreate(0, &wi);
  gwinSetFont(modeLabel, dv24Font);

  wi.g.y += wi.g.height+5;
  wi.g.height = 35;


  wi.text = "NA";
  heaterLabel = gwinLabelCreate(0, &wi);
  gwinSetFont(heaterLabel, dv32aaFont);

	// Checkboxes - for the 2nd checkbox we apply special drawing before making it visible
	wi.g.parent = ghPgCheckboxes;
	wi.g.width = CHECKBOX_WIDTH; wi.g.height = CHECKBOX_HEIGHT; wi.g.x = 5;
	wi.g.y = 5; wi.text = "C1";
	ghCheckbox1 = gwinCheckboxCreate(0, &wi);
	wi.customDraw = 0; //gwinCheckboxDraw_CheckOnRight;
	wi.g.y += wi.g.height+10; wi.text = "C2";
	ghCheckbox2 = gwinCheckboxCreate(0, &wi);
	wi.customDraw = 0; wi.g.width = DISABLEALL_WIDTH;
	wi.g.y += wi.g.height+10; wi.text = "Disable All";
	ghCheckDisableAll = gwinCheckboxCreate(0, &wi);

    // Labels
	wi.g.parent = ghPgLabels;
	wi.g.width = gwinGetInnerWidth(ghPgLabels)-10;	wi.g.height = LABEL_HEIGHT;
	wi.g.x = wi.g.y = 5; wi.text = "N/A";
	ghLabelSlider1 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider1, 100, "Slider 1:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelSlider2 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider2, 100, "Slider 2:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelSlider3 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider3, 100, "Slider 3:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelSlider4 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelSlider4, 100, "Slider 4:");
	wi.g.y += LABEL_HEIGHT + 2;
	ghLabelRadio1 = gwinLabelCreate(0, &wi);
	gwinLabelSetAttribute(ghLabelRadio1, 100, "RadioButton 1:");


	// Radio Buttons
	wi.g.parent = ghPgRadios;
	wi.g.width = RADIO_WIDTH; wi.g.height = RADIO_HEIGHT; wi.g.y = 5;
	wi.g.x = 5; wi.text = "Yes";
	ghRadio1 = gwinRadioCreate(0, &wi, GROUP_YESNO);
	wi.g.x += wi.g.width; wi.text = "No"; if (wi.g.x + wi.g.width > gwinGetInnerWidth(ghPgRadios)) { wi.g.x = 5; wi.g.y += RADIO_HEIGHT; }
	ghRadio2 = gwinRadioCreate(0, &wi, GROUP_YESNO);
	gwinRadioPress(ghRadio1);
	wi.g.width = COLOR_WIDTH; wi.g.y += RADIO_HEIGHT+5;
	wi.g.x = 5; wi.text = "Black";
	ghRadioBlack = gwinRadioCreate(0, &wi, GROUP_COLORS);
	wi.g.x += wi.g.width; wi.text = "White"; if (wi.g.x + wi.g.width > gwinGetInnerWidth(ghPgRadios)) { wi.g.x = 5; wi.g.y += RADIO_HEIGHT; }
	ghRadioWhite = gwinRadioCreate(0, &wi, GROUP_COLORS);
	wi.g.x += wi.g.width; wi.text = "Yellow"; if (wi.g.x + wi.g.width > gwinGetInnerWidth(ghPgRadios)) { wi.g.x = 5; wi.g.y += RADIO_HEIGHT; }
	ghRadioYellow = gwinRadioCreate(0, &wi, GROUP_COLORS);
	gwinRadioPress(ghRadioWhite);
/*
	// Lists
	border = gwinGetInnerWidth(ghPgLists) < 10+2*LIST_WIDTH ? 2 : 5;
	wi.g.parent = ghPgLists;
	wi.g.width = LIST_WIDTH; wi.g.height = LIST_HEIGHT; wi.g.y = border;
	wi.g.x = border; wi.text = "L1";
	ghList1 = gwinListCreate(0, &wi, FALSE);
	gwinListAddItem(ghList1, "Item 0", FALSE);
	gwinListAddItem(ghList1, "Item 1", FALSE);
	gwinListAddItem(ghList1, "Item 2", FALSE);
	gwinListAddItem(ghList1, "Item 3", FALSE);
	gwinListAddItem(ghList1, "Item 4", FALSE);
	gwinListAddItem(ghList1, "Item 5", FALSE);
	gwinListAddItem(ghList1, "Item 6", FALSE);
	gwinListAddItem(ghList1, "Item 7", FALSE);
	gwinListAddItem(ghList1, "Item 8", FALSE);
	gwinListAddItem(ghList1, "Item 9", FALSE);
	gwinListAddItem(ghList1, "Item 10", FALSE);
	gwinListAddItem(ghList1, "Item 11", FALSE);
	gwinListAddItem(ghList1, "Item 12", FALSE);
	gwinListAddItem(ghList1, "Item 13", FALSE);
	wi.text = "L2"; wi.g.x += LIST_WIDTH+border; if (wi.g.x + LIST_WIDTH > gwinGetInnerWidth(ghPgLists)) { wi.g.x = border; wi.g.y += LIST_HEIGHT+border; }
	ghList2 = gwinListCreate(0, &wi, TRUE);
	gwinListAddItem(ghList2, "Item 0", FALSE);
	gwinListAddItem(ghList2, "Item 1", FALSE);
	gwinListAddItem(ghList2, "Item 2", FALSE);
	gwinListAddItem(ghList2, "Item 3", FALSE);
	gwinListAddItem(ghList2, "Item 4", FALSE);
	gwinListAddItem(ghList2, "Item 5", FALSE);
	gwinListAddItem(ghList2, "Item 6", FALSE);
	gwinListAddItem(ghList2, "Item 7", FALSE);
	gwinListAddItem(ghList2, "Item 8", FALSE);
	gwinListAddItem(ghList2, "Item 9", FALSE);
	gwinListAddItem(ghList2, "Item 10", FALSE);
	gwinListAddItem(ghList2, "Item 11", FALSE);
	gwinListAddItem(ghList2, "Item 12", FALSE);
	gwinListAddItem(ghList2, "Item 13", FALSE);
	wi.text = "L3"; wi.g.x += LIST_WIDTH+border; if (wi.g.x + LIST_WIDTH > gwinGetInnerWidth(ghPgLists)) { wi.g.x = border; wi.g.y += LIST_HEIGHT+border; }
	ghList3 = gwinListCreate(0, &wi, TRUE);
	gwinListAddItem(ghList3, "Item 0", FALSE);
	gwinListAddItem(ghList3, "Item 1", FALSE);
	gwinListAddItem(ghList3, "Item 2", FALSE);
	gwinListAddItem(ghList3, "Item 3", FALSE);
	gdispImageOpenFile(&imgYesNo, "image_yesno.gif");
	gwinListItemSetImage(ghList3, 1, &imgYesNo);
	gwinListItemSetImage(ghList3, 3, &imgYesNo);
	wi.text = "L4"; wi.g.x += LIST_WIDTH+border; if (wi.g.x + LIST_WIDTH > gwinGetInnerWidth(ghPgLists)) { wi.g.x = border; wi.g.y += LIST_HEIGHT+border; }
	ghList4 = gwinListCreate(0, &wi, TRUE);
	gwinListAddItem(ghList4, "Item 0", FALSE);
	gwinListAddItem(ghList4, "Item 1", FALSE);
	gwinListAddItem(ghList4, "Item 2", FALSE);
	gwinListAddItem(ghList4, "Item 3", FALSE);
	gwinListAddItem(ghList4, "Item 4", FALSE);
	gwinListAddItem(ghList4, "Item 5", FALSE);
	gwinListAddItem(ghList4, "Item 6", FALSE);
	gwinListAddItem(ghList4, "Item 7", FALSE);
	gwinListAddItem(ghList4, "Item 8", FALSE);
	gwinListAddItem(ghList4, "Item 9", FALSE);
	gwinListAddItem(ghList4, "Item 10", FALSE);
	gwinListAddItem(ghList4, "Item 11", FALSE);
	gwinListAddItem(ghList4, "Item 12", FALSE);
	gwinListAddItem(ghList4, "Item 13", FALSE);
	gwinListSetScroll(ghList4, scrollSmooth);
*/
	// Image
	wi.g.parent = ghPgImages;
	wi.g.x = wi.g.y = 0; wi.g.width = gwinGetInnerWidth(ghPgImages); wi.g.height = gwinGetInnerHeight(ghPgImages);
	ghImage1 = gwinImageCreate(0, &wi.g);
	gwinImageOpenFile(ghImage1, "romfs_img_ugfx.gif");

	// Progressbar
/*
	wi.g.parent = ghPgProgressbars;
	wi.g.width = gwinGetInnerWidth(ghPgImages)-10; wi.g.height = SLIDER_WIDTH; wi.g.y = 5;
	wi.g.x = 5; wi.text = "Progressbar 1";
	ghProgressbar1 = gwinProgressbarCreate(0, &wi);
	gwinProgressbarSetResolution(ghProgressbar1, 10);
*/
}

/**
 * Set the visibility of widgets based on which tab is selected.
 */
static void setTab(GHandle tab) {
	/* Make sure everything is invisible first */
	gwinHide(ghPgControls);
	gwinHide(ghPgCheckboxes);
	gwinHide(ghPgLabels);
	gwinHide(ghPgRadios);
	gwinHide(ghPgImages);
	//gwinHide(ghPgProgressbars);
	// Stop the progress bar
//	gwinProgressbarStop(ghProgressbar1);
	//gwinProgressbarReset(ghProgressbar1);

	/* Turn on widgets depending on the tab selected */
	if (tab == ghTabButtons) {
		gwinShow(ghPgControls);
	//} else if (tab == ghTabSliders) {
	//	gwinShow(ghPgSliders);
	} else if (tab == ghTabCheckboxes) {
		gwinShow(ghPgCheckboxes);
	} else if (tab == ghTabLabels) {
		gwinShow(ghPgLabels);
	} else if (tab == ghTabRadios) {
		gwinShow(ghPgRadios);
//	} else if (tab == ghTabLists) {
//		gwinShow(ghPgLists);
	} else if (tab == ghTabImages) {
		gwinShow(ghPgImages);

	//} else if (tab == ghTabProgressbar) {
	//	gwinShow(ghPgProgressbars);

		// Start the progress bar
		//gwinProgressbarStart(ghProgressbar1, 500);
	}


  if(tab == ghTabImages)
  {
    gwinHide(ghConsole);
  }
  else
  {
    gwinShow(ghConsole);
  }

}

/**
 * Set the enabled state of every widget (except the tabs etc)
 */
static void setEnabled(bool_t ena) {
	gwinSetEnabled(ghPgControls, ena);
//	gwinSetEnabled(ghPgSliders, ena);
	gwinSetEnabled(ghPgLabels, ena);
	gwinSetEnabled(ghPgRadios, ena);
	//gwinSetEnabled(ghPgLists, ena);
	gwinSetEnabled(ghPgImages, ena);
	//gwinSetEnabled(ghPgProgressbars, ena);
	// Checkboxes we need to do individually so we don't disable the checkbox to re-enable everything
	gwinSetEnabled(ghCheckbox1, ena);
	gwinSetEnabled(ghCheckbox2, ena);
	//gwinSetEnabled(ghCheckDisableAll, TRUE);
}

// Incoming JSON data from IO board
void incomming_io_board(const char *str);
static SerialConfig serialCfg =
{
  115200, // bit rate
};

static void init_link(void)
{
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(7)); //TX
  palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(7)); //RX
  sdStart(&SD1, &serialCfg);
}
/*
static int io_board_power = 0;
static int io_board_temperature = 67;
static int io_board_mode = 0;
static int io_board_pump_state = 0;



static void io_board_mock_out(void)
{
  json_t *root = json_object();
  json_t *modeString = json_string(io_board_mode ? "Auto" : "Manual");
  json_object_set_new(root, "Mode", modeString);
  char tmpStr[16];

  sprintf(tmpStr, "%d%c",io_board_mode ? io_board_temperature : io_board_power, io_board_mode ? 'C' : '%');

  json_object_set_new(root, "Ref", json_string(tmpStr));
  json_object_set_new(root, "PumpTemp", json_integer(io_board_power));
  json_object_set_new(root, "PumpState", json_string(io_board_pump_state ? "On" : "Off"));

  char *jdump = json_dumps(root, 0);
  // Send JSON data to UI
  incomming_io_board(jdump);

  //Release buffer
  chHeapFree(jdump);

  json_decref(root);
}

// Simulate incomming JSON data to io board from UI
static void io_board_mock_in(const char *str)
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
        io_board_power += io_board_power < 100;
      }
      else if(strcmp(command, "Down") == 0)
      {
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
  // Reply to UI
  io_board_mock_out();

}
*/




static void notify_io_board(json_t *root)
{
  char *jdump = json_dumps(root, 0);
  // Send JSON data to IO board
  //sdWrite(&SD1, (uint8_t *)jdump, strlen(jdump)+1);
 // io_board_mock_in(jdump);
  io_ifc_in(jdump);
  //Release buffer
  chHeapFree(jdump);
}

// Incoming JSON data from IO board
void incomming_io_board(const char *str)
{
  json_error_t error;
  json_t *root = json_loads(str, 0, &error);

  if(root == 0)
  {
    gwinPrintf(ghConsole, "json_loads error %s\n", error.text);
    return;
  }

  const char *key;
  json_t *value;
  char tmpStr[16];

  json_object_foreach(root, key, value)
  {
    //gwinPrintf(ghConsole, "%s\n", key);

    if(strcmp(key,"Mode") == 0)
    {
      //gwinPrintf(ghConsole, "%s\n", json_string_value(key));
      gwinSetText(modeLabel, json_string_value(value), true);
    }

    else if(strcmp(key,"Ref") == 0)
    {
      gwinSetText(heaterLabel, json_string_value(value), true);
    }
    else if(strcmp(key,"PumpTemp") == 0)
    {
      int i = json_integer_value(value);
      sprintf(tmpStr, "%d.%dC", i / 10, i % 10);
      gwinSetText(pumpTempValue, tmpStr, true);
    }
    else if(strcmp(key,"PumpState") == 0)
    {
      gwinSetText(ghPumpOnOffBtn, json_string_value(value), true);
      if(strcmp(json_string_value(value), "On") == 0)
        gwinSetStyle(ghPumpOnOffBtn, &OnOffWidgetStyleOn);
      else
        gwinSetStyle(ghPumpOnOffBtn, &OnOffWidgetStyleOff);
    }
  }
  json_decref(root);
}




static char rxBuffer[512];
static uint16_t rxIdx = 0;
 // Serial receiver thread
static msg_t thRx(void *arg)
{
  (void)arg;
  chRegSetThreadName("Receiver");
  EventListener elSerData;
  flagsmask_t flags;
  chEvtRegisterMask((EventSource *)chnGetEventSource(&SD1), &elSerData, EVENT_MASK(1));


  while (TRUE)
  {
     chEvtWaitOne(EVENT_MASK(1));
     flags = chEvtGetAndClearFlags(&elSerData);
     if (flags & CHN_INPUT_AVAILABLE)
     {
        msg_t charbuf;
        do
        {
           charbuf = chnGetTimeout(&SD1, TIME_IMMEDIATE);
           if ( charbuf != Q_TIMEOUT )
           {
             if(rxIdx < sizeof(rxBuffer))
             {
               rxBuffer[rxIdx] = charbuf;
               rxIdx++;
             }

             if(charbuf == 0)
             {
               //io_board_mock_in(rxBuffer);
               incomming_io_board(rxBuffer);

               rxIdx = 0;
             }

            // chSequentialStreamPut(&SD1, charbuf);
           }
        }
        while (charbuf != Q_TIMEOUT);
     }
  }

  return 0;
}

static void send_command(json_t *root, json_t *commandString, const char *command)
{
  json_string_set(commandString, command);
  notify_io_board(root);
 // json_decref(root);
}

int gui_test_main(void) {

  // Initialize and clear the display
  gfxInit();
	GEvent *			pe;

	// Initialize the display
	//gfxInit();

	// Connect the mouse
	//#if GINPUT_NEED_MOUSE
	//	gwinAttachMouse(0);
	//#endif

  chRegSetThreadName("GUI_input");
  //init_link();
  //chThdCreateFromHeap(NULL, THD_WA_SIZE(4096), NORMALPRIO, thRx, NULL);
	// Set the widget defaults
	font = gdispOpenFont("*");			// Get the first defined font.
	gwinSetDefaultFont(font);
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(White);

	// Create the gwin windows/widgets
	createWidgets();

    // Assign toggles and dials to specific buttons & sliders etc.
	#if GINPUT_NEED_TOGGLE
		gwinAttachToggle(ghModeButton, 0, 0);
		gwinAttachToggle(ghTempSetpointUpBtn, 0, 1);
	#endif
	#if GINPUT_NEED_DIAL
		gwinAttachDial(ghSlider1, 0, 0);
		gwinAttachDial(ghSlider3, 0, 1);
	#endif

	// Make the console visible
	gwinShow(ghConsole);
	gwinClear(ghConsole);

    // We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	// Press the Tab we want visible
	gwinRadioPress(ghTabButtons);

  json_t *root = json_object();
  json_t *commandString = json_string("None");
  json_object_set_new(root, "Command", commandString);
  //json_t *powerInt = json_integer(24);
  //json_object_set_new(root, "Power", powerInt);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, 1000);
    if(pe == NULL)
    {
      send_command(root, commandString, "Poll");
      continue;
    }
		switch(pe->type) {
		case GEVENT_GWIN_BUTTON:
		{

			GEventGWinButton *we = (GEventGWinButton *)pe;

			if(we->gwin == ghModeButton)
      {
        send_command(root, commandString, "Mode");

        //gwinSetText(modeLabel, json_string_value(commandString), false);
        //json_decref(root); // Release JSON object
      }
			else if(we->gwin == ghTempSetpointUpBtn)
      {
        send_command(root, commandString, "Up");
      }
      else if(we->gwin == ghTempSetpointDownBtn)
      {
        send_command(root, commandString,  "Down");
      }
      else if(we->gwin == ghPumpOnOffBtn)
      {
        send_command(root, commandString,  "PumpCtrl");
      }

			//gwinPrintf(ghConsole, "Button %s\n", gwinGetText(((GEventGWinButton *)pe)->gwin));

    } break;

			case GEVENT_GWIN_SLIDER:
			gwinPrintf(ghConsole, "Slider %s=%d\n", gwinGetText(((GEventGWinSlider *)pe)->gwin), ((GEventGWinSlider *)pe)->position);
			break;

		case GEVENT_GWIN_CHECKBOX:
			gwinPrintf(ghConsole, "Checkbox %s=%s\n", gwinGetText(((GEventGWinCheckbox *)pe)->gwin), ((GEventGWinCheckbox *)pe)->isChecked ? "Checked" : "UnChecked");

			// If it is the Disable All checkbox then do that.
			if (((GEventGWinCheckbox *)pe)->gwin == ghCheckDisableAll) {
				gwinPrintf(ghConsole, "%s All\n", ((GEventGWinCheckbox *)pe)->isChecked ? "Disable" : "Enable");
				setEnabled(!((GEventGWinCheckbox *)pe)->isChecked);
			}
			break;

		case GEVENT_GWIN_LIST:
			gwinPrintf(ghConsole, "List %s Item %d %s\n", gwinGetText(((GEventGWinList *)pe)->gwin), ((GEventGWinList *)pe)->item,
					gwinListItemIsSelected(((GEventGWinList *)pe)->gwin, ((GEventGWinList *)pe)->item) ? "Selected" : "Unselected");
			break;

		case GEVENT_GWIN_RADIO:
			gwinPrintf(ghConsole, "Radio Group %u=%s\n", ((GEventGWinRadio *)pe)->group, gwinGetText(((GEventGWinRadio *)pe)->gwin));

			switch(((GEventGWinRadio *)pe)->group) {
			case GROUP_TABS:

				// Set control visibility depending on the tab selected
				setTab(((GEventGWinRadio *)pe)->gwin);

				// We show the state of some of the GUI elements here
				if (((GEventGWinRadio *)pe)->gwin == ghTabLabels) {
/*
					char tmp[20];

					// The sliders
					snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider1));
					gwinSetText(ghLabelSlider1, tmp, TRUE);
					snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider2));
					gwinSetText(ghLabelSlider2, tmp, TRUE);
					snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider3));
					gwinSetText(ghLabelSlider3, tmp, TRUE);
					snprintg(tmp, sizeof(tmp), "%d%%", gwinSliderGetPosition(ghSlider4));
					gwinSetText(ghLabelSlider4, tmp, TRUE);
*/
					// The radio buttons
					if (gwinRadioIsPressed(ghRadio1)) {
 						gwinSetText(ghLabelRadio1, "Yes", TRUE);
 					} else if (gwinRadioIsPressed(ghRadio2)) {
 						gwinSetText(ghLabelRadio1, "No", TRUE);
 					}
				}
				break;
/*
			case GROUP_COLORS:
				{
					const GWidgetStyle	*pstyle;

					gwinPrintf(ghConsole, "Change Color Scheme\n");

					if (((GEventGWinRadio *)pe)->gwin == ghRadioYellow)
						pstyle = &YellowWidgetStyle;
					else if (((GEventGWinRadio *)pe)->gwin == ghRadioBlack)
						pstyle = &BlackWidgetStyle;
					else
						pstyle = &WhiteWidgetStyle;

					// Clear the screen to the new color
					#if GDISP_NEED_CLIP
						gdispUnsetClip();
					#endif
					gdispClear(pstyle->background);

					// Update the style on all controls
					gwinSetDefaultStyle(pstyle, TRUE);
				}
				break;
*/
			}

			break;

		default:
			gwinPrintf(ghConsole, "Unknown %d\n", pe->type);
			break;
		}
	}
	return 0;
}

static int graphX = 0;
static char buf[16];
static int oldItemp = 0;

void gui_graph_new_value(float temp)
{
/*
  io_board_temperature = temp * 10;

  if(currentView != ghPageHome)
    return;

  int itemp = (int)(temp*10);

  gdispUnsetClip();
  gwinGraphDrawPoint(graph, graphX,  (itemp/4)-60);
  if(++graphX >= gwinGetWidth(graph))
  {
    graphX = 0;
  }

  if(oldItemp != itemp)
  {
    oldItemp = itemp;
    snprintf(buf, sizeof(buf), "%d.%d", itemp/10, itemp%10);
    //snprintf(buf, sizeof(buf), "%d.%d", tnow.tm_year, tnow.tm_mon);
    gwinSetText(ghLabel1, buf,FALSE);

  }
*/
}
