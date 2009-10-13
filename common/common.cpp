/**********************************************************/
/* Routines d'affichage de parametres et caracteristiques */
/**********************************************************/
#include "fctsys.h"
#include "gr_basic.h"
#include "trigo.h"
#include "wxstruct.h"
#include "base_struct.h"
#include "common.h"
#include "macros.h"
#include "build_version.h"
#include "confirm.h"
#include <wx/process.h>

/**
 * Global variables definitions.
 *
 * TODO: All if these variables should be moved into the class were they
 *       are defined and used.  Most of them probably belong in the
 *       application class.
 */

/* Standard page sizes in 1/1000 inch */
Ki_PageDescr  g_Sheet_A4( wxSize( 11700, 8267 ), wxPoint( 0, 0 ), wxT( "A4" ) );
Ki_PageDescr  g_Sheet_A3( wxSize( 16535, 11700 ), wxPoint( 0, 0 ), wxT( "A3" ) );
Ki_PageDescr  g_Sheet_A2( wxSize( 23400, 16535 ), wxPoint( 0, 0 ), wxT( "A2" ) );
Ki_PageDescr  g_Sheet_A1( wxSize( 33070, 23400 ), wxPoint( 0, 0 ), wxT( "A1" ) );
Ki_PageDescr  g_Sheet_A0( wxSize( 46800, 33070 ), wxPoint( 0, 0 ), wxT( "A0" ) );
Ki_PageDescr  g_Sheet_A( wxSize( 11000, 8500 ), wxPoint( 0, 0 ), wxT( "A" ) );
Ki_PageDescr  g_Sheet_B( wxSize( 17000, 11000 ), wxPoint( 0, 0 ), wxT( "B" ) );
Ki_PageDescr  g_Sheet_C( wxSize( 22000, 17000 ), wxPoint( 0, 0 ), wxT( "C" ) );
Ki_PageDescr  g_Sheet_D( wxSize( 34000, 22000 ), wxPoint( 0, 0 ), wxT( "D" ) );
Ki_PageDescr  g_Sheet_E( wxSize( 44000, 34000 ), wxPoint( 0, 0 ), wxT( "E" ) );
Ki_PageDescr  g_Sheet_GERBER( wxSize( 32000, 32000 ), wxPoint( 0, 0 ),
                             wxT( "GERBER" ) );
Ki_PageDescr  g_Sheet_user( wxSize( 17000, 11000 ), wxPoint( 0, 0 ),
                           wxT( "User" ) );

Ki_PageDescr* g_SheetSizeList[NB_ITEMS + 1] =
{
    &g_Sheet_A4,   &g_Sheet_A3, &g_Sheet_A2, &g_Sheet_A1, &g_Sheet_A0,
    &g_Sheet_A,    &g_Sheet_B,  &g_Sheet_C,  &g_Sheet_D,  &g_Sheet_E,
    &g_Sheet_user, NULL
};


/* File extension definitions.  Please do not changes these.  If a different
 * file extension is needed, create a new definition in the application.
 * Please note, just because they are defined as const dosen't guarentee
 * that they cannot be changed. */
const wxString ProjectFileExtension( wxT( "pro" ) );
const wxString SchematicFileExtension( wxT( "sch" ) );
const wxString BoardFileExtension( wxT( "brd" ) );
const wxString NetlistFileExtension( wxT( "net" ) );
const wxString GerberFileExtension( wxT( "pho" ) );
const wxString PdfFileExtension( wxT( "pdf" ) );

/* Proper wxFileDialog wild card definitions. */
const wxString ProjectFileWildcard( _( "Kicad project files (*.pro)|*.pro" ) );
const wxString BoardFileWildcard( _( "Kicad PCB files (*.brd)|*.brd" ) );
const wxString SchematicFileWildcard( _( "Kicad schematic files (*.sch)|*.sch" ) );
const wxString NetlistFileWildcard( _( "Kicad netlist files (*.net)|*.net" ) );
const wxString GerberFileWildcard( _( "Gerber files (*.pho)|*.pho" ) );
const wxString PdfFileWildcard( _( "Portable document format files (*.pdf)|*.pdf" ) );
const wxString AllFilesWildcard( _( "All files (*)|*" ) );


wxString       g_ProductName    = wxT( "KiCad E.D.A.  " );
bool           g_ShowPageLimits = true;
int            g_GridColor = DARKGRAY;
wxString       g_UserLibDirBuffer;
int            g_DebugLevel;
int            g_MouseOldButtons;
int            g_KeyPressed;

// Nom (full file name) du file Configuration par defaut (kicad.pro)
wxString       g_Prj_Default_Config_FullFilename;

// Nom du file Configuration local (<curr projet>.pro)
wxString       g_Prj_Config_LocalFilename;

// Handle the preferd editor for browsing report files:
int            g_UnitMetric; // display units mm = 1, inches = 0, cm = 2

/* Draw color for moving objects: */
int            g_GhostColor;

/* predefined colors used in kicad.
 * Please: if you change a value, remember these values are carefully chosen
 * to have good results in pcbnew, that uses the ORed value of basic colors
 * when displaying superimposed objects
 * This list must have exactly NBCOLOR items
 */
StructColors ColorRefs[NBCOLOR] =
{
    { 0,    0,   0,   BLACK,        wxT( "BLACK" ),        DARKDARKGRAY          },
    { 192,  0,   0,   BLUE,         wxT( "BLUE" ),         LIGHTBLUE             },
    { 0,    160, 0,   GREEN,        wxT( "GREEN" ),        LIGHTGREEN            },
    { 160,  160, 0,   CYAN,         wxT( "CYAN" ),         LIGHTCYAN             },
    { 0,    0,   160, RED,          wxT( "RED" ),          LIGHTRED              },
    { 160,  0,   160, MAGENTA,      wxT( "MAGENTA" ),      LIGHTMAGENTA          },
    { 0,    128, 128, BROWN,        wxT( "BROWN" ),        YELLOW                },
    { 192,  192, 192, LIGHTGRAY,    wxT( "GRAY" ),         WHITE                 },
    { 128,  128, 128, DARKGRAY,     wxT( "DARKGRAY" ),     LIGHTGRAY             },
    { 255,  0,   0,   LIGHTBLUE,    wxT( "LIGHTBLUE" ),    LIGHTBLUE             },
    { 0,    255, 0,   LIGHTGREEN,   wxT( "LIGHTGREEN" ),   LIGHTGREEN            },
    { 255,  255, 0,   LIGHTCYAN,    wxT( "LIGHTCYAN" ),    LIGHTCYAN             },
    { 0,    0,   255, LIGHTRED,     wxT( "LIGHTRED" ),     LIGHTRED              },
    { 255,  0,   255, LIGHTMAGENTA, wxT( "LIGHTMAGENTA" ), LIGHTMAGENTA          },
    { 0,    255, 255, YELLOW,       wxT( "YELLOW" ),       YELLOW                },
    { 255,  255, 255, WHITE,        wxT( "WHITE" ),        WHITE                 },
    {  64,  64,  64,  DARKDARKGRAY, wxT( "DARKDARKGRAY" ), DARKGRAY              },
    {  64,  0,   0,   DARKBLUE,     wxT( "DARKBLUE" ),     BLUE                  },
    {    0, 64,  0,   DARKGREEN,    wxT( "DARKGREEN" ),    GREEN                 },
    {  64,  64,  0,   DARKCYAN,     wxT( "DARKCYAN" ),     CYAN                  },
    {    0, 0,   80,  DARKRED,      wxT( "DARKRED" ),      RED                   },
    {  64,  0,   64,  DARKMAGENTA,  wxT( "DARKMAGENTA" ),  MAGENTA               },
    {    0, 64,  64,  DARKBROWN,    wxT( "DARKBROWN" ),    BROWN                 },
    {  128, 255, 255, LIGHTYELLOW,  wxT( "LIGHTYELLOW" ),  LIGHTYELLOW           }
};


/** Function GetBuildVersion()
 * Return the build date
 */
wxString GetBuildVersion()
{
    return g_BuildVersion;
}


/** Function GetAboutBuildVersion()
 * Return custom build date for about dialog
 */
wxString GetAboutBuildVersion()
{
    return g_BuildAboutVersion;
}


/** function SetLocaleTo_C_standard
 * because kicad is internationalized, switch internatization to "C" standard
 * i.e. uses the . (dot) as separator in print/read float numbers
 * (some contries (France, Germany ..) use , (comma) as separator)
 * This function must be called before read or write ascii files using float numbers in data
 * the SetLocaleTo_C_standard function must be called after reading or writing the file
 *
 * This is wrapper to the C setlocale( LC_NUMERIC, "C" ) function,
 * but could make more easier an optional use of locale in kicad
 */
/********************************/
void SetLocaleTo_C_standard( void )
{
    setlocale( LC_NUMERIC, "C" );    // Switch the locale to standard C
}


/** function SetLocaleTo_Default
 * because kicad is internationalized, switch internatization to default
 * to use the default separator in print/read float numbers
 * (. (dot) but some contries (France, Germany ..) use , (comma) as separator)
 * This function must be called after a call to SetLocaleTo_C_standard
 *
 * This is wrapper to the C setlocale( LC_NUMERIC, "" ) function,
 * but could make more easier an optional use of locale in kicad
 */
/********************************/
void SetLocaleTo_Default( void )
/********************************/
{
    setlocale( LC_NUMERIC, "" );      // revert to the current locale
}


/********************************************************************/
bool EnsureTextCtrlWidth( wxTextCtrl*     aCtrl,
                          const wxString* aString )
/********************************************************************/
{
    wxWindow* window = aCtrl->GetParent();

    if( !window )
        window = aCtrl;

    wxString ctrlText;

    if( !aString )
    {
        ctrlText = aCtrl->GetValue();
        aString  = &ctrlText;
    }

    wxCoord width;
    wxCoord height;

    {
        wxClientDC dc( window );
        dc.SetFont( aCtrl->GetFont() );
        dc.GetTextExtent( *aString, &width, &height );
    }

    wxSize size = aCtrl->GetSize();
    if( size.GetWidth() < width + 10 )
    {
        size.SetWidth( width + 10 );
        aCtrl->SetSizeHints( size );
        return true;
    }
    return false;
}


/*********************************************************************************************/
Ki_PageDescr::Ki_PageDescr( const wxSize&   size,
                            const wxPoint&  offset,
                            const wxString& name )
/*********************************************************************************************/
{
    // All sizes are in 1/1000 inch
    m_Size   = size;
    m_Offset = offset;
    m_Name   = name;

    // Adjust the default value for margins to 400 mils (0,4 inch or 10 mm)
#if defined(KICAD_GOST)
    m_LeftMargin   = LEFTMARGIN;
    m_RightMargin  = RIGHTMARGIN;
    m_TopMargin    = TOPMARGIN;
    m_BottomMargin = BOTTOMMARGIN;
#else
    m_LeftMargin = m_RightMargin = m_TopMargin = m_BottomMargin = 400;
#endif
}


/************************************/
wxString ReturnUnitSymbol( int Units )
/************************************/
{
    wxString label;

    switch( Units )
    {
    case INCHES:
        label = _( " (\"):" );
        break;

    case MILLIMETRE:
        label = _( " (mm):" );
        break;

    default:
        break;
    }

    return label;
}


/*
 * Add string "  (mm):" or " ("):" to the static text Stext.
 *  Used in dialog boxes for entering values depending on selected units
 */
/**************************************************/
void AddUnitSymbol( wxStaticText& Stext, int Units )
/**************************************************/
{
    wxString msg = Stext.GetLabel();
    msg += ReturnUnitSymbol( Units );

    Stext.SetLabel( msg );
}


/*
 * Convert the number Value in a string according to the internal units
 *  and the selected unit (g_UnitMetric) and put it in the wxTextCtrl TextCtrl
 */
/******************************************/
void PutValueInLocalUnits( wxTextCtrl& TextCtr, int Value, int Internal_Unit )
/*****************************************/
{
    wxString msg = ReturnStringFromValue( g_UnitMetric, Value, Internal_Unit );

    TextCtr.SetValue( msg );
}


/*
 * Convert the Value in the wxTextCtrl TextCtrl in an integer,
 *  according to the internal units and the selected unit (g_UnitMetric)
 */
/***************************************************/
int ReturnValueFromTextCtrl( const wxTextCtrl& TextCtr, int Internal_Unit )
/***************************************************/
{
    int      value;
    wxString msg = TextCtr.GetValue();

    value = ReturnValueFromString( g_UnitMetric, msg, Internal_Unit );

    return value;
}


/** Function ReturnStringFromValue
 * Return the string from Value, according to units (inch, mm ...) for display,
 * and the initial unit for value
 * @param aUnit = display units (INCHES, MILLIMETRE ..)
 * @param aValue = value in Internal_Unit
 * @param aInternal_Unit = units per inch for Value
 * @param aAdd_unit_symbol = true to add symbol unit to the string value
 * @return a wxString what contains value and optionnaly the sumbol unit (like 2.000 mm)
 */
/*******************************************/
wxString ReturnStringFromValue( int aUnits, int aValue, int aInternal_Unit,
                                bool aAdd_unit_symbol )
/*******************************************/
{
    wxString StringValue;
    double   value_to_print;

    if( aUnits >= CENTIMETRE )
        StringValue << aValue;
    else
    {
        value_to_print = To_User_Unit( (bool) aUnits, (double) aValue, aInternal_Unit );
        StringValue.Printf( ( aInternal_Unit > 1000 ) ? wxT( "%.4f" ) : wxT( "%.3f" ),
                           value_to_print );
    }

    if( aAdd_unit_symbol )
        switch( aUnits )
        {
        case INCHES:
            StringValue += _( " \"" );
            break;

        case MILLIMETRE:
            StringValue += _( " mm" );
            break;

        default:
            break;
        }

    return StringValue;
}


/*
 * Return the string from Value, according to units (inch, mm ...) for display,
 *  and the initial unit for value
 *  Unit = display units (INCH, MM ..)
 *  Value = text
 *  Internal_Unit = units per inch for computed value
 */
/****************************************************************************/
int ReturnValueFromString( int Units, const wxString& TextValue,
                           int Internal_Unit )
/****************************************************************************/
{
    int    Value;
    double dtmp = 0;

    TextValue.ToDouble( &dtmp );
    if( Units >= CENTIMETRE )
        Value = wxRound( dtmp );
    else
        Value = From_User_Unit( (bool) Units, dtmp, Internal_Unit );

    return Value;
}


/**
 * Function wxStringSplit
 * Split a String to a String List when founding 'splitter'
 * @return the list
 * @param txt : wxString : a String text
 * @param splitter : wxChar : the 'split' character
 */
/**********************************************************/
wxArrayString* wxStringSplit( wxString txt, wxChar splitter )
/**********************************************************/
{
    wxArrayString* list = new wxArrayString();

    while( 1 )
    {
        int index = txt.Find( splitter );
        if( index == wxNOT_FOUND )
            break;

        wxString tmp;
        tmp = txt.Mid( 0, index );
        txt = txt.Mid( index + 1, txt.size() - index );
        list->Add( tmp );
    }

    if( !txt.IsEmpty() )
    {
        list->Add( txt );
    }

    return list;
}


/******************************************************************/
double To_User_Unit( bool is_metric, double val, int internal_unit_value )
/******************************************************************/

/**
 * Function To_User_Unit
 * Convert in inch or mm the variable "val" (double)given in internal units
 * @return the converted value, in double
 * @param is_metric : true if the result must be returned in mm , false if inches
 * @param val : double : the given value
 * @param internal_unit_value = internal units per inch
 */
{
    double value;

    if( is_metric )
        value = val * 25.4 / internal_unit_value;
    else
        value = val / internal_unit_value;

    return value;
}


/*
 * Return in internal units the value "val" given in inch or mm
 */
/*****************************************/
int From_User_Unit( bool is_metric, double val, int internal_unit_value )
/*****************************************/
{
    double value;

    if( is_metric )
        value = val * internal_unit_value / 25.4;
    else
        value = val * internal_unit_value;

    return wxRound( value );
}


/*
 * Return the string date "day month year" like "23 jun 2005"
 */
/********/
wxString GenDate()
/********/
{
    static const wxString mois[12] =
    {
        wxT( "jan" ), wxT( "feb" ), wxT( "mar" ), wxT( "apr" ), wxT( "may" ), wxT( "jun" ),
        wxT( "jul" ), wxT( "aug" ), wxT( "sep" ), wxT( "oct" ), wxT( "nov" ), wxT( "dec" )
    };

    time_t     buftime;
    struct tm* Date;
    wxString   string_date;

    time( &buftime );
    Date = gmtime( &buftime );
    string_date.Printf( wxT( "%d %s %d" ), Date->tm_mday,
                        GetChars( mois[Date->tm_mon] ),
                        Date->tm_year + 1900 );
    return string_date;
}


/*
 * My memory allocation
 */
/***********************************/
void* MyMalloc( size_t nb_octets )
/***********************************/
{
    void* pt_mem;

    if( nb_octets == 0 )
    {
        DisplayError( NULL, wxT( "Allocate 0 bytes !!" ) );
        return NULL;
    }
    pt_mem = malloc( nb_octets );
    if( pt_mem == NULL )
    {
        wxString msg;
        msg.Printf( wxT( "Out of memory: allocation %d bytes" ), nb_octets );
        DisplayError( NULL, msg );
    }
    return pt_mem;
}


/**
 * Function ProcessExecute
 * runs a child process.
 * @param aCommandLine The process and any arguments to it all in a single string.
 * @param aFlags The same args as allowed for wxExecute()
 * @return bool - true if success, else false
 */
/********************************************/
bool ProcessExecute( const wxString& aCommandLine, int aFlags )
/********************************************/
{
#ifdef __WINDOWS__
    int        pid = wxExecute( aCommandLine );
    return pid ? true : false;
#else
    wxProcess* process = wxProcess::Open( aCommandLine, aFlags );
    return (process != NULL) ? true : false;
#endif
}


/*
 * My memory allocation, memory space is cleared
 */
/*****************************/
void* MyZMalloc( size_t nb_octets )
/*****************************/
{
    void* pt_mem = MyMalloc( nb_octets );

    if( pt_mem )
        memset( pt_mem, 0, nb_octets );
    return pt_mem;
}


/*******************************/
void MyFree( void* pt_mem )
/*******************************/
{
    if( pt_mem )
        free( pt_mem );
}


/*
 * Return the name of the layer number "layer_number".
 *  if omitSpacePadding == TRUE, the name can be used for a file name
 *  (no spaces, replaced by _)
 */
/**************************************************************/
wxString ReturnPcbLayerName( int layer_number, bool omitSpacePadding )
/**************************************************************/
{
    const unsigned LAYER_LIMIT = 29;

    // These are only default layer names.  For PCBNEW, the copper names
    // may be over-ridden in the BOARD file *.brd.

    static const wxString layer_name_list[] =
    {
        _( "Copper   " ), _( "Inner L1 " ), _( "Inner L2 " ), _( "Inner L3 " ),
        _( "Inner L4 " ), _( "Inner L5 " ), _( "Inner L6 " ), _( "Inner L7 " ),
        _( "Inner L8 " ), _( "Inner L9 " ), _( "Inner L10" ), _( "Inner L11" ),
        _( "Inner L12" ), _( "Inner L13" ), _( "Inner L14" ), _( "Component" ),
        _( "Adhes Cop" ), _( "Adhes Cmp" ), _( "SoldP Cop" ), _( "SoldP Cmp" ),
        _( "SilkS Cop" ), _( "SilkS Cmp" ), _( "Mask Cop " ), _( "Mask Cmp " ),
        _( "Drawings " ), _( "Comments " ), _( "Eco1     " ), _( "Eco2     " ),
        _( "Edges Pcb" ), _( "BAD INDEX" )
    };

    if( (unsigned) layer_number > LAYER_LIMIT )
        layer_number = LAYER_LIMIT;

    const wxString* p = &layer_name_list[layer_number];

    if( omitSpacePadding )
    {
        wxString ret = *p;  // copy the string

        // modify the copy
        ret.Trim();
        ret.Replace( wxT( " " ), wxT( "_" ) );

        return ret;
    }
    else
        return *p;
}


enum textbox {
    ID_TEXTBOX_LIST = 8010
};

BEGIN_EVENT_TABLE( WinEDA_TextFrame, wxDialog )
EVT_LISTBOX_DCLICK( ID_TEXTBOX_LIST, WinEDA_TextFrame::D_ClickOnList )
EVT_LISTBOX( ID_TEXTBOX_LIST, WinEDA_TextFrame::D_ClickOnList )
EVT_CLOSE( WinEDA_TextFrame::OnClose )
END_EVENT_TABLE()

/***************************************************************************/
WinEDA_TextFrame::WinEDA_TextFrame( wxWindow*       parent,
                                    const wxString& title ) :
    wxDialog( parent,
              -1, title,
              wxPoint( -1, -1 ),
              wxSize( 250, 350 ),
              wxDEFAULT_DIALOG_STYLE |
              wxFRAME_FLOAT_ON_PARENT |
              MAYBE_RESIZE_BORDER )
/***************************************************************************/
{
    /*
     * TODO background and foreground colors of WinEDA_TextFrame should be
     * controllable / settable with project settings or config file and not
     * hardcoded in binairy !
     */

    wxSize size;

    m_Parent = parent;

    CentreOnParent();

    size   = GetClientSize();
    m_List = new wxListBox( this,
                            ID_TEXTBOX_LIST,
                            wxPoint( 0, 0 ),
                            size,
                            0, NULL,
                            wxLB_ALWAYS_SB | wxLB_SINGLE );

    /* The color of the text in the wxListBox (black) */
    m_List->SetBackgroundColour( wxColour( 255, 255, 255 ) );

    /* The foreground color of the wxListBox (white) */
    m_List->SetForegroundColour( wxColour( 0, 0, 0 ) );

    SetReturnCode( -1 );
}


/***************************************************/
void WinEDA_TextFrame::Append( const wxString& text )
/***************************************************/
{
    m_List->Append( text );
}


/**********************************************************/
void WinEDA_TextFrame::D_ClickOnList( wxCommandEvent& event )
/**********************************************************/
{
    int ii = m_List->GetSelection();

    EndModal( ii );
}


/*************************************************/
void WinEDA_TextFrame::OnClose( wxCloseEvent& event )
/*************************************************/
{
    EndModal( -1 );
}


/**
 *  Routine d'affichage d'un parametre.
 *  pos_X = cadrage horizontal
 *      si pos_X < 0 : la position horizontale est la derniere
 *          valeur demandee >= 0
 *  texte_H = texte a afficher en ligne superieure.
 *      si "", par d'affichage sur cette ligne
 *  texte_L = texte a afficher en ligne inferieure.
 *      si "", par d'affichage sur cette ligne
 *  color = couleur d'affichage
 */
/*****************************************************************************/
void Affiche_1_Parametre( WinEDA_DrawFrame* frame, int pos_X,
                          const wxString& texte_H, const wxString& texte_L,
                          int color )
/*****************************************************************************/
{
    frame->MsgPanel->Affiche_1_Parametre( pos_X, texte_H, texte_L, color );
}


/***********************/
int GetTimeStamp()
/***********************/

/*
 *  Retourne une identification temporelle (Time stamp) differente a chaque appel
 */
{
    static int OldTimeStamp, NewTimeStamp;

    NewTimeStamp = time( NULL );
    if( NewTimeStamp <= OldTimeStamp )
        NewTimeStamp = OldTimeStamp + 1;
    OldTimeStamp = NewTimeStamp;
    return NewTimeStamp;
}


/**
 *  TODO replace this obsolete funtion by ReturnStringFromValue
 * Retourne pour affichage la valeur d'un parametre, selon type d'unites choisies
 *  entree : valeur en mils , buffer de texte
 *  retourne en buffer : texte : valeur exprimee en pouces ou millimetres
 *                      suivie de " ou mm
 */
/*********************************************/
const wxString& valeur_param( int valeur, wxString& buf_texte )
/*********************************************/
{
    if( g_UnitMetric )
    {
        buf_texte.Printf( wxT( "%3.3f mm" ), valeur * 0.00254 );
    }
    else
    {
        buf_texte.Printf( wxT( "%2.4f \"" ), valeur * 0.0001 );
    }

    return buf_texte;
}


/*
 *
 */
/**********************************/
wxString& operator <<( wxString& aString, const wxPoint& aPos )
/*********************************/
{
    wxString temp;

    aString << wxT( "@ (" ) << valeur_param( aPos.x, temp );
    aString << wxT( "," ) << valeur_param( aPos.y, temp );
    aString << wxT( ")" );

    return aString;
}
