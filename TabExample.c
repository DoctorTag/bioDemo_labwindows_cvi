/**************************************************************************************/
/*                                                                                    */
/*  FILE:       tabexample.c                                                          */
/*                                                                                    */
/*  PURPOSE:    Shows how to how to programmatically configure and                    */
/*              change some characteristics of the tab control. Specific examples     */
/*              include changing the names of the pages, changing the tab page        */
/*              label properties, hiding pages, setting and dimming the close button, */
/*              and coloring individual pages different colors.                       */
/*                                                                                    */
/**************************************************************************************/

#include <analysis.h>
#include <cvirte.h>
#include <userint.h>
#include <ansi_c.h>
#include "TabExample.h"

static int panelHandle;
static int TabPanelHandle, gOrigPageBkgndColor;

#define PAGE_ONE 0
#define PAGE_TWO 1

static void PulsePattern(int n, double amp, int delay, int width, double pulse[]);
static void TrianglePattern(int n, double amp, double tri[]);
static void UpdateTabPageOneBkndColor(void);
static void UpdateTabPageTwoBkndColor(void);
static void UpdateConformToSystemTheme(void);

int main (int argc, char *argv[])
{
    if (InitCVIRTE (0, argv, 0) == 0)
        return -1;  /* out of memory */
    if ((panelHandle = LoadPanel (0, "TabExample.uir", PANEL)) < 0)
        return -1;

    GetTabPageAttribute (panelHandle, PANEL_TAB, PAGE_ONE, ATTR_BACKCOLOR, &gOrigPageBkgndColor);
    SetCtrlVal (panelHandle, PANEL_BKGND_COLOR_1, gOrigPageBkgndColor);
    SetCtrlVal (panelHandle, PANEL_BKGND_COLOR_2, gOrigPageBkgndColor);

    DisplayPanel (panelHandle);
    RunUserInterface ();
    DiscardPanel (panelHandle);
    return 0;
}


/********************************************************************/
/*   The TabsControlCB function dims controls corresponding to the  */
/*   closed tab page.                                               */
/********************************************************************/
int CVICALLBACK TabsControlCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int numberOfPages;
	char secondPageName[256];
	int checked;

    switch (event)
    {
        case EVENT_TAB_PAGE_CLOSE:
            GetNumTabPages (panelHandle, PANEL_TAB, &numberOfPages);
            switch (numberOfPages)
			{
				case 1:
					SetCtrlAttribute (panelHandle, PANEL_FIRST_PAGE_NAME, ATTR_DIMMED, 1);
					SetCtrlAttribute (panelHandle, PANEL_BKGND_COLOR_1, ATTR_DIMMED, 1);
					SetCtrlAttribute (panelHandle, PANEL_CHECKBOXONE, ATTR_DIMMED, 1);
					SetCtrlAttribute (panelHandle, PANEL_CHKBOX_DIMMED_ONE, ATTR_DIMMED, 1);
					SetCtrlAttribute (panelHandle, PANEL_ACTIVETAB_RING, ATTR_DIMMED, 1);
					DeleteListItem (panelHandle, PANEL_ACTIVETAB_RING, 0, 1);
					break;
				case 2:
					SetCtrlAttribute (panelHandle, PANEL_SECOND_PAGE_NAME, ATTR_DIMMED, 1);
					if (eventData1 == 0) {
						GetCtrlVal (panelHandle, PANEL_SECOND_PAGE_NAME, secondPageName); 
						SetCtrlVal (panelHandle, PANEL_FIRST_PAGE_NAME, secondPageName);
						GetCtrlVal (panelHandle, PANEL_BKGND_COLOR_2, &checked);
						SetCtrlVal (panelHandle, PANEL_BKGND_COLOR_1, checked);
						GetCtrlVal (panelHandle, PANEL_CHECKBOXTWO, &checked);
						SetCtrlVal (panelHandle, PANEL_CHECKBOXONE, checked);
						GetCtrlVal (panelHandle, PANEL_CHKBOX_DIMMED_TWO, &checked);
						SetCtrlVal (panelHandle, PANEL_CHKBOX_DIMMED_ONE, checked);
					}
					SetCtrlAttribute (panelHandle, PANEL_BKGND_COLOR_2, ATTR_DIMMED, 1);
					SetCtrlAttribute (panelHandle, PANEL_CHECKBOXTWO, ATTR_DIMMED, 1);
					SetCtrlAttribute (panelHandle, PANEL_CHKBOX_DIMMED_TWO, ATTR_DIMMED, 1);
					DeleteListItem (panelHandle, PANEL_ACTIVETAB_RING, eventData1, 1);
					break;
			}
            break;
    }
    return 0;
}

/****************************************************************************/
/*   The UpdateNameOneCB function acquires the values entered in the first  */
/*   and second page new name string. It then uses those values to set      */
/*   the Tab Page Attribute Label Text for each of the pages.               */
/****************************************************************************/
int CVICALLBACK UpdateNameOneCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    char firstPageName[256];

    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_FIRST_PAGE_NAME, firstPageName);
            ReplaceListItem (panelHandle, PANEL_ACTIVETAB_RING, PAGE_ONE,
                firstPageName, PAGE_ONE);
            SetTabPageAttribute (panelHandle, PANEL_TAB, PAGE_ONE,
                ATTR_LABEL_TEXT, firstPageName);

            break;
    }
    return 0;
}

/****************************************************************************/
/*   The UpdateNameTwoCB function acquires the values entered in the first  */
/*   and second page new name string. It then uses those values to set      */
/*   the Tab Page Attribute Label Text for each of the pages.               */
/****************************************************************************/
int CVICALLBACK UpdateNameTwoCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    char secondPageName[256];

    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_SECOND_PAGE_NAME, secondPageName);
            ReplaceListItem (panelHandle, PANEL_ACTIVETAB_RING,
                PAGE_TWO, secondPageName, PAGE_TWO);
            SetTabPageAttribute (panelHandle, PANEL_TAB, PAGE_TWO,
                ATTR_LABEL_TEXT, secondPageName);
            break;
    }
    return 0;
}

/****************************************************************************/
/*  TabOneColorCB sets the color of the first page based on the user input  */
/*  value. The function uses the SetTabPageAttribute function to change     */
/*  the background color by specifying the attribute to be background       */
/*  color.                                                                  */
/****************************************************************************/
int CVICALLBACK TabOneColorCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
			UpdateTabPageOneBkndColor();
			SetCtrlVal (panelHandle, PANEL_USE_WIN_STYLE, 0);
			UpdateConformToSystemTheme();
            break;
    }
    return 0;
}

/****************************************************************************/
/*  TabTwoColorCB sets the color of the second page based on the user input */
/*  value. The function uses the SetTabPageAttribute function to change     */
/*  the background color by specifying the attribute to be background       */
/*  color.                                                                  */
/****************************************************************************/
int CVICALLBACK TabTwoColorCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
			UpdateTabPageTwoBkndColor();
			SetCtrlVal (panelHandle, PANEL_USE_WIN_STYLE, 0);
			UpdateConformToSystemTheme();
            break;
    }
    return 0;
}

/**************************************************************************/
/*  CloseButtonVisibleCB turns on/off the visibility of the close button. */
/**************************************************************************/
int CVICALLBACK CloseButtonVisibleCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int closeButtonVisible;
	switch (event)
		{
		case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_CLOSE_VISIBLE, &closeButtonVisible);
            SetCtrlAttribute (panelHandle, PANEL_TAB, ATTR_TABS_CLOSE_BUTTON_VISIBLE, closeButtonVisible);
            break;
		}
	return 0;
}

/***********************************************************************/
/*  UseWindowsStyleCB turns on/off the theming of the panel.           */
/***********************************************************************/
int CVICALLBACK UseWindowsStyleCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			UpdateConformToSystemTheme();
			SetCtrlVal(panelHandle, PANEL_BKGND_COLOR_1, gOrigPageBkgndColor);
			SetCtrlVal(panelHandle, PANEL_BKGND_COLOR_2, gOrigPageBkgndColor);
			UpdateTabPageOneBkndColor();
			UpdateTabPageTwoBkndColor();
			break;
		}
	return 0;
}

/***********************************************************************/
/*  UpdateConformToSystemTheme updates the panel attribute             */
/*  ATTR_CONFORM_TO_SYSTEM_THEME based on the value of the             */
/*  PANEL_USE_WIN_STYLE checkbox                                       */
/***********************************************************************/
static void UpdateConformToSystemTheme(void)
{
	int conform;
	GetCtrlVal(panelHandle, PANEL_USE_WIN_STYLE, &conform);
	SetPanelAttribute(panelHandle, ATTR_CONFORM_TO_SYSTEM_THEME, conform);
}

/***********************************************************************/
/*  UpdateTabPageOneBkndColor updates the tab page attribute           */
/*  ATTR_BACKCOLOR based on the value of the PANEL_BKGND_COLOR_1       */
/*  color numeric                                                      */
/***********************************************************************/
static void UpdateTabPageOneBkndColor(void)
{
	int pageColor, numberOfPages = 0;
	GetNumTabPages (panelHandle, PANEL_TAB, &numberOfPages);
	if (numberOfPages > 0)
	{
		GetCtrlVal (panelHandle, PANEL_BKGND_COLOR_1, &pageColor);
		SetTabPageAttribute (panelHandle, PANEL_TAB, PAGE_ONE, ATTR_BACKCOLOR, pageColor);
	}
}

/***********************************************************************/
/*  UpdateTabPageTwoBkndColor updates the tab page attribute           */
/*  ATTR_BACKCOLOR based on the value of the PANEL_BKGND_COLOR_2       */
/*  color numeric                                                      */
/***********************************************************************/
static void UpdateTabPageTwoBkndColor(void)
{
	int pageColor, numberOfPages = 0;
	GetNumTabPages (panelHandle, PANEL_TAB, &numberOfPages);
	if (numberOfPages > 1)
	{
		GetCtrlVal (panelHandle, PANEL_BKGND_COLOR_2, &pageColor);
	    SetTabPageAttribute (panelHandle, PANEL_TAB, PAGE_TWO, ATTR_BACKCOLOR, pageColor);
	}
}

/***********************************************************************/
/*  TabOneVisibilityCB turns on/off the visibility of the first page   */
/*  depending on the state of the LED.                                 */
/***********************************************************************/
int CVICALLBACK TabOneVisibilityCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int visible;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_CHECKBOXONE, &visible);
            SetTabPageAttribute (panelHandle, PANEL_TAB, 
                PAGE_ONE, ATTR_VISIBLE, visible);

            break;
    }
    return 0;
}

/***********************************************************************/
/*  TabTwoVisibilityCB turns on/off the visibility of the second page  */
/*  depending on the state of the LED.                                 */
/***********************************************************************/
int CVICALLBACK TabTwoVisibilityCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int visible;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_CHECKBOXTWO, &visible);
            SetTabPageAttribute (panelHandle, PANEL_TAB, PAGE_TWO, ATTR_VISIBLE, visible);

            break;
    }
    return 0;
}

/*****************************************************************************/
/*  TabOneCloseDimmedCB makes dimmed/undimmed the close button of the first  */
/*  page depending on the state of the LED.                                  */
/*****************************************************************************/
int CVICALLBACK TabOneCloseDimmedCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int closeButtonDimmed;
	switch (event)
		{
		case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_CHKBOX_DIMMED_ONE, &closeButtonDimmed);
            SetTabPageAttribute (panelHandle, PANEL_TAB,
				PAGE_ONE, ATTR_TABS_CLOSE_BUTTON_DIMMED, closeButtonDimmed);
            break;
		}
	return 0;
}

/*****************************************************************************/
/*  TabTwoCloseDimmedCB makes dimmed/undimmed the close button of the second */
/*  page depending on the state of the LED.                                  */
/*****************************************************************************/
int CVICALLBACK TabTwoCloseDimmedCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int closeButtonDimmed;
	switch (event)
		{
		case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_CHKBOX_DIMMED_TWO, &closeButtonDimmed);
            SetTabPageAttribute (panelHandle, PANEL_TAB,
				PAGE_TWO, ATTR_TABS_CLOSE_BUTTON_DIMMED, closeButtonDimmed);
            break;
		}
	return 0;
}

/***********************************************************************/
/*  The TabLocationCB callback function sets the location of the tabs  */
/*  using the SetCtrlAttribute to the value specified in the ring      */
/*  control.                                                           */
/***********************************************************************/
int CVICALLBACK TabLocationCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int tabLocation;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_TAB_LOCATION, &tabLocation);
            SetCtrlAttribute (panelHandle, PANEL_TAB, ATTR_TABS_LOCATION, tabLocation);
            break;
    }
    return 0;
}

/*************************************************************************/
/*  TabFitModeCB sets the mode on which to place the tabs by using the   */
/*  value set on the Tabs Fit Mode ring control.                         */
/*************************************************************************/
int CVICALLBACK TabFitModeCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int tabfitMode;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_TAB_FIT_MODE, &tabfitMode);
            SetCtrlAttribute (panelHandle, PANEL_TAB, ATTR_TABS_FIT_MODE, tabfitMode);
            break;
    }
    return 0;
}

/*************************************************************************/
/*  BoldLabelCB sets the Tab's labels to bold depending on the state of  */
/*  the 'B' control.                                                     */
/*************************************************************************/
int CVICALLBACK BoldLabelCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int boldState;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_BOLD_LABEL_BUTTON, &boldState);
            SetTabPageAttribute (panelHandle, PANEL_TAB, VAL_ALL_OBJECTS,
                ATTR_LABEL_BOLD, boldState);
            break;
    }
    return 0;
}

/*************************************************************************/
/*  ItalicLabelCB sets the Tab's labels to italic depending on the state */
/*  of the 'B' control.                                                  */
/*************************************************************************/
int CVICALLBACK ItalicLabelCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int italicState;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_ITALIC_LABEL_BUTTON, &italicState);
            SetTabPageAttribute (panelHandle, PANEL_TAB, VAL_ALL_OBJECTS,
                ATTR_LABEL_ITALIC, italicState);
            break;
    }
    return 0;
}

/*************************************************************************/
/*  UnderlinedLabelCB sets the Tab's labels underlined depending on the  */
/*  state of the 'B' control.                                            */
/*************************************************************************/
int CVICALLBACK UnderlinedLabelCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int underLineState;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_ULINE_LABEL_BUTTON, &underLineState);
            SetTabPageAttribute (panelHandle, PANEL_TAB, VAL_ALL_OBJECTS,
                ATTR_LABEL_UNDERLINE, underLineState);
            break;
    }
    return 0;
}

/***********************************************************************/
/*  LabelSizeCB sets the size of the Tab's labels to the size entered  */
/*  in the numeric control.                                            */
/***********************************************************************/
int CVICALLBACK LabelSizeCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int labelSize;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_LABEL_PNT_SIZE, &labelSize);
            SetTabPageAttribute (panelHandle, PANEL_TAB, VAL_ALL_OBJECTS,
                ATTR_LABEL_POINT_SIZE, labelSize);

            break;
    }
    return 0;
}

/***************************************************************************/
/*   LabelColorCB sets the color of the Tab's labels to the value selected */
/*   on the color numeric control.                                         */
/***************************************************************************/
int CVICALLBACK LabelColorCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int labelSize;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal (panelHandle, PANEL_LABEL_COLOR, &labelSize);
            SetTabPageAttribute (panelHandle, PANEL_TAB, VAL_ALL_OBJECTS,
                ATTR_LABEL_COLOR, labelSize);

            break;
    }
    return 0;
}


/***********************************************************************/
/*   Generates a waveform depending on the value selected on the ring  */
/*   control.                                                          */
/***********************************************************************/
int CVICALLBACK GenerateWfm (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int signal = 0;
    double waveform[1000];
    switch (event)
    {
        case EVENT_COMMIT:
  
            GetPanelHandleFromTabPage (panelHandle, PANEL_TAB, PAGE_ONE, &TabPanelHandle);
            GetCtrlVal (TabPanelHandle, TABPANEL_SGN_TYPE, &signal);

            if (signal == 0)
                SinePattern (1000, 10.0, 0.0, 1.0, waveform);
            else if (signal == 1)
                PulsePattern (1000, 10.0, 100, 500, waveform);
            else
                TrianglePattern (1000, 10.0, waveform);

            DeleteGraphPlot (TabPanelHandle, TABPANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
            PlotY (TabPanelHandle, TABPANEL_GRAPH, waveform, 1000, VAL_DOUBLE,
                VAL_THIN_LINE, VAL_EMPTY_SQUARE,VAL_SOLID, 1, VAL_GREEN);
   
            break;
    }
    return 0;
}

/***********************************************************************/
/*  KnobCB sets the value of the numeric tank depending on the         */
/*  value indicated on the knob.                                       */
/***********************************************************************/
int CVICALLBACK KnobCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    double knobValue;
    switch (event)
    {
        case EVENT_VAL_CHANGED:
            GetPanelHandleFromTabPage (panelHandle, PANEL_TAB, PAGE_TWO, &TabPanelHandle);
            GetCtrlVal (TabPanelHandle, TABPANEL_2_NUMERICKNOB, &knobValue);
            SetCtrlVal (TabPanelHandle, TABPANEL_2_NUMERICTANK, knobValue);

            break;
    }
    return 0;
}

/***********************************************************************/
/*  ActiveTabCB gets the value selected on the Ring control and sets   */
/*  the active page to that value.                                     */
/***********************************************************************/
int CVICALLBACK ActiveTabCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    int selectedTab;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlIndex (panelHandle, PANEL_ACTIVETAB_RING, &selectedTab);
            SetActiveTabPage (panelHandle, PANEL_TAB, selectedTab);
            break;
    }
    return 0;
}

/***********************************************************************/
/*  QuitCB stops the application from running.                         */
/***********************************************************************/
int CVICALLBACK QuitCB (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
            QuitUserInterface (0);
            break;
    }
    return 0;
}

/***********************************************************************/
/*  Generates a pulse pattern.                                         */
/***********************************************************************/
static void PulsePattern(int n, double amp, int delay, int width, double pulse[])
{
	int i;

	memset(pulse, 0, n * sizeof(*pulse));
	for (i = delay; i < delay + width; ++i)
		pulse[i] = amp;
}

/***********************************************************************/
/*  Generates a triangle pattern.                                      */
/***********************************************************************/
static void TrianglePattern(int n, double amp, double tri[])
{
	int i;

	if (n % 2 == 0)
	{
		for (i = 0; i < n; ++i)
			tri[i] = (amp * (1 - abs(2 * i - n))) / n;
	}
	else
	{
		for (i = 0; i < n; ++i)
			tri[i] = (amp * (1 - abs(2 * i - n + 1))) / (n - 1);
	}
}

