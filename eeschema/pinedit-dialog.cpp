/////////////////////////////////////////////////////////////////////////////
// Name:        pinedit-dialog.cpp
// Purpose:
// Author:      jean-pierre Charras
// Modified by:
// Created:     11/02/2006 13:30:59
// RCS-ID:
// Copyright:   License GNU
// Licence:
/////////////////////////////////////////////////////////////////////////////

// Generated by DialogBlocks (unregistered), 11/02/2006 13:30:59

////@begin includes
////@end includes
#include "confirm.h"

#include "pinedit-dialog.h"

////@begin XPM images
////@end XPM images

void InstallPineditFrame( WinEDA_LibeditFrame* parent, wxDC* DC,
                          const wxPoint & pos )
{
    wxPoint MousePos = parent->GetScreen()->m_Curseur;
    int accept = TRUE;

    if ( ( parent->GetDrawItem() == NULL )
         || ( parent->GetDrawItem()->Type() == COMPONENT_PIN_DRAW_TYPE ) )
    {
        LIB_PIN* Pin = (LIB_PIN*) parent->GetDrawItem();
        WinEDA_PinPropertiesFrame dlg( parent );
        accept = dlg.ShowModal();

        if ( !accept && Pin && ( Pin->m_Flags & IS_NEW ) )
        {
            if ( parent->DrawPanel->ForceCloseManageCurseur && DC )
                parent->DrawPanel->ForceCloseManageCurseur( parent->DrawPanel,
                                                            DC );
        }
    }
    else
        DisplayError( parent, wxT( "Error: Not a Pin!" ) );

    parent->GetScreen()->m_Curseur = MousePos;
    parent->DrawPanel->MouseToCursorSchema();
}

/*!
 * WinEDA_PinPropertiesFrame type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WinEDA_PinPropertiesFrame, wxDialog )

/*!
 * WinEDA_PinPropertiesFrame event table definition
 */

BEGIN_EVENT_TABLE( WinEDA_PinPropertiesFrame, wxDialog )

////@begin WinEDA_PinPropertiesFrame event table entries
    EVT_SPIN_UP( ID_SPINBUTTON_INC_DEC_PINSIZE, WinEDA_PinPropertiesFrame::OnSpinbuttonIncDecPinsizeUp )
    EVT_SPIN_DOWN( ID_SPINBUTTON_INC_DEC_PINSIZE, WinEDA_PinPropertiesFrame::OnSpinbuttonIncDecPinsizeDown )

    EVT_BUTTON( wxID_CANCEL, WinEDA_PinPropertiesFrame::OnCancelClick )

    EVT_BUTTON( wxID_OK, WinEDA_PinPropertiesFrame::OnOkClick )

////@end WinEDA_PinPropertiesFrame event table entries

END_EVENT_TABLE()

/*!
 * WinEDA_PinPropertiesFrame constructors
 */

WinEDA_PinPropertiesFrame::WinEDA_PinPropertiesFrame( )
{
}

WinEDA_PinPropertiesFrame::WinEDA_PinPropertiesFrame( WinEDA_LibeditFrame* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    LIB_PIN* CurrentPin = (LIB_PIN*) parent->GetDrawItem();

    m_Parent = parent;
    if ( CurrentPin )
    {
        m_Parent->InitEditOnePin();
    }
    Create(parent, id, caption, pos, size, style);

    /* Init dialog pin name and pin name size values */
    SetValuesInDialog( );
}

/*!
 * WinEDA_PinPropertiesFrame creator
 */

bool WinEDA_PinPropertiesFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WinEDA_PinPropertiesFrame member initialisation
    m_PinNameCtrl = NULL;
    m_PinNumCtrl = NULL;
    m_PinSizeCtrl = NULL;
    m_PinSizeIncDecButton = NULL;
    m_PinSizeText = NULL;
    m_CommonUnit = NULL;
    m_CommonConvert = NULL;
    m_NoDraw = NULL;
    m_PinNameSizeText = NULL;
    m_PinNameSizeCtrl = NULL;
    m_PinNumSizeText = NULL;
    m_PinNumSizeCtrl = NULL;
    m_PinOrient = NULL;
    m_btClose = NULL;
    m_PinShape = NULL;
    m_PinElectricalType = NULL;
////@end WinEDA_PinPropertiesFrame member initialisation

////@begin WinEDA_PinPropertiesFrame creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end WinEDA_PinPropertiesFrame creation
    return true;
}

/*!
 * Control creation for WinEDA_PinPropertiesFrame
 */

void WinEDA_PinPropertiesFrame::CreateControls()
{
////@begin WinEDA_PinPropertiesFrame content construction
    // Generated by DialogBlocks, 01/05/2009 16:19:57 (unregistered)

    WinEDA_PinPropertiesFrame* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, _("Pin Name :"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText4, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_PinNameCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_PINNAME, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(m_PinNameCtrl, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, _("Pin Num :"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_PinNumCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_PINNUM, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(m_PinNumCtrl, 0, wxGROW|wxALL, 5);

    itemBoxSizer3->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer9Static = new wxStaticBox(itemDialog1, wxID_ANY, _(" Pin Options :"));
    wxStaticBoxSizer* itemStaticBoxSizer9 = new wxStaticBoxSizer(itemStaticBoxSizer9Static, wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer9, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer9->Add(itemBoxSizer10, 0, wxGROW, 5);

    m_PinSizeCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_PINSIZE, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer10->Add(m_PinSizeCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PinSizeIncDecButton = new wxSpinButton( itemDialog1, ID_SPINBUTTON_INC_DEC_PINSIZE, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
    m_PinSizeIncDecButton->SetRange(0, 2000);
    m_PinSizeIncDecButton->SetValue(0);
    itemBoxSizer10->Add(m_PinSizeIncDecButton, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_PinSizeText = new wxStaticText( itemDialog1, wxID_STATIC, _("Pin Lenght"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer9->Add(m_PinSizeText, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_CommonUnit = new wxCheckBox( itemDialog1, ID_CHECKBOX_COMMON_UNITS, _("Common to Units"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_CommonUnit->SetValue(false);
    itemStaticBoxSizer9->Add(m_CommonUnit, 0, wxALIGN_LEFT|wxALL, 5);

    m_CommonConvert = new wxCheckBox( itemDialog1, ID_CHECKBOX_COMMON_CONVERT, _("Common to convert"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_CommonConvert->SetValue(false);
    itemStaticBoxSizer9->Add(m_CommonConvert, 0, wxALIGN_LEFT|wxALL, 5);

    m_NoDraw = new wxCheckBox( itemDialog1, ID_CHECKBOX_NOT_DRAWN, _("No Draw"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_NoDraw->SetValue(false);
    itemStaticBoxSizer9->Add(m_NoDraw, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer17, 0, wxGROW|wxALL, 5);

    m_PinNameSizeText = new wxStaticText( itemDialog1, wxID_STATIC, _("Size"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(m_PinNameSizeText, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_PinNameSizeCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_PINNAME_SIZE, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(m_PinNameSizeCtrl, 0, wxALIGN_LEFT|wxALL, 5);

    m_PinNumSizeText = new wxStaticText( itemDialog1, wxID_STATIC, _("Size"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(m_PinNumSizeText, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_PinNumSizeCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_PINNUM_SIZE, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(m_PinNumSizeCtrl, 0, wxALIGN_LEFT|wxALL, 5);

    itemBoxSizer17->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxArrayString m_PinOrientStrings;
    m_PinOrientStrings.Add(_("Right"));
    m_PinOrientStrings.Add(_("Left"));
    m_PinOrientStrings.Add(_("Up"));
    m_PinOrientStrings.Add(_("Down"));
    m_PinOrient = new wxRadioBox( itemDialog1, ID_RADIOBOX, _("Pin Orient:"), wxDefaultPosition, wxDefaultSize, m_PinOrientStrings, 1, wxRA_SPECIFY_COLS );
    m_PinOrient->SetSelection(0);
    itemBoxSizer17->Add(m_PinOrient, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer24, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer24->Add(itemBoxSizer25, 0, wxGROW|wxALL, 5);

    m_btClose = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(m_btClose, 0, wxGROW|wxALL, 5);

    wxButton* itemButton27 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemButton27->SetDefault();
    itemBoxSizer25->Add(itemButton27, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer24->Add(itemBoxSizer28, 0, wxGROW|wxALL, 5);

    wxArrayString m_PinShapeStrings;
    m_PinShapeStrings.Add(_("line"));
    m_PinShapeStrings.Add(_("invert"));
    m_PinShapeStrings.Add(_("clock"));
    m_PinShapeStrings.Add(_("clock inv"));
    m_PinShapeStrings.Add(_("low in"));
    m_PinShapeStrings.Add(_("low clock"));
    m_PinShapeStrings.Add(_("low out"));
    m_PinShape = new wxRadioBox( itemDialog1, ID_RADIOBOX1, _("Pin Shape:"), wxDefaultPosition, wxDefaultSize, m_PinShapeStrings, 1, wxRA_SPECIFY_COLS );
    m_PinShape->SetSelection(0);
    itemBoxSizer28->Add(m_PinShape, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_PinElectricalTypeStrings;
    m_PinElectricalTypeStrings.Add(_("Input"));
    m_PinElectricalTypeStrings.Add(_("Output"));
    m_PinElectricalTypeStrings.Add(_("Bidi"));
    m_PinElectricalTypeStrings.Add(_("3 States"));
    m_PinElectricalTypeStrings.Add(_("Passive"));
    m_PinElectricalTypeStrings.Add(_("Unspecified"));
    m_PinElectricalTypeStrings.Add(_("Power In"));
    m_PinElectricalTypeStrings.Add(_("Power Out"));
    m_PinElectricalTypeStrings.Add(_("Open coll"));
    m_PinElectricalTypeStrings.Add(_("Open emit"));
    m_PinElectricalType = new wxRadioBox( itemDialog1, ID_RADIOBOX2, _("Electrical Type:"), wxDefaultPosition, wxDefaultSize, m_PinElectricalTypeStrings, 1, wxRA_SPECIFY_COLS );
    m_PinElectricalType->SetSelection(0);
    itemBoxSizer28->Add(m_PinElectricalType, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end WinEDA_PinPropertiesFrame content construction

    m_btClose->SetFocus();
}

/*!
 * Should we show tooltips?
 */

bool WinEDA_PinPropertiesFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap WinEDA_PinPropertiesFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin WinEDA_PinPropertiesFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end WinEDA_PinPropertiesFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon WinEDA_PinPropertiesFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin WinEDA_PinPropertiesFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end WinEDA_PinPropertiesFrame icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void WinEDA_PinPropertiesFrame::OnOkClick( wxCommandEvent& event )
{
    PinPropertiesAccept(event);
    EndModal(1);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void WinEDA_PinPropertiesFrame::OnCancelClick( wxCommandEvent& event )
{
    EndModal(0);
}

/*******************************************************/
void WinEDA_PinPropertiesFrame::SetValuesInDialog(void)
/*******************************************************/
{
    wxString number;
    LIB_PIN* CurrentPin = (LIB_PIN*) m_Parent->GetDrawItem();
    wxString msg;
    int tmp, ii;

    // Init values and texts for pin name:
    if ( CurrentPin ) msg = CurrentPin->m_PinName;
    else msg = wxEmptyString;
    m_PinNameCtrl->SetValue(msg);
    tmp = CurrentPin ? CurrentPin->m_PinNameSize : LastPinNameSize;
    msg = ReturnStringFromValue(g_UnitMetric, tmp, m_Parent->m_InternalUnits);
    m_PinNameSizeCtrl->SetValue(msg);
    msg = m_PinNameSizeText->GetLabel() + ReturnUnitSymbol();
    m_PinNameSizeText->SetLabel(msg);

    /* Init dialog pin num and pin num size values */
    if ( CurrentPin ) CurrentPin->ReturnPinStringNum(msg);
    else msg = wxEmptyString;
    m_PinNumCtrl->SetValue(msg);
    tmp = CurrentPin ? CurrentPin->m_PinNumSize : LastPinNumSize;
    msg = ReturnStringFromValue(g_UnitMetric, tmp, m_Parent->m_InternalUnits);
    m_PinNumSizeCtrl->SetValue(msg);
    msg = m_PinNumSizeText->GetLabel() + ReturnUnitSymbol();
    m_PinNumSizeText->SetLabel(msg);

    /* init dialog pin size */
    msg = m_PinSizeText->GetLabel() + ReturnUnitSymbol();
    m_PinSizeText->SetLabel(msg);

    int pinsize = CurrentPin ? CurrentPin->m_PinLen : LastPinSize;
    msg = ReturnStringFromValue(g_UnitMetric, pinsize, m_Parent->m_InternalUnits);
    m_PinSizeCtrl->SetValue(msg);
    m_PinSizeIncDecButton->SetValue(pinsize);

    /* init pin options */
    if ( CurrentPin )
        {
        if ( CurrentPin->m_Unit == 0 ) m_CommonUnit->SetValue(TRUE);
        }
    else m_CommonUnit->SetValue(LastPinCommonUnit);
    if ( CurrentPin )
        {
        if ( CurrentPin->m_Convert == 0 ) m_CommonConvert->SetValue(TRUE);
        }
    else m_CommonConvert->SetValue(LastPinCommonConvert);
    if ( CurrentPin )
        {
        if ( CurrentPin->m_Attributs & PINNOTDRAW ) m_NoDraw->SetValue(TRUE);
        }
    else m_NoDraw->SetValue(LastPinNoDraw);

    tmp = CurrentPin ? CurrentPin->m_Orient : LastPinOrient;
    switch ( tmp )
    {
        case PIN_RIGHT:
            m_PinOrient->SetSelection(0);
            break;

        case PIN_LEFT:
            m_PinOrient->SetSelection(1);
            break;

        case PIN_UP:
            m_PinOrient->SetSelection(2);
            break;

        case PIN_DOWN:
            m_PinOrient->SetSelection(3);
            break;
    }

    tmp = CurrentPin ? CurrentPin->m_PinShape : LastPinShape;
    m_PinShape->SetSelection( 0 );
    for ( ii = 0; ii < NBSHAPES; ii++ )
    {
        if ( CodeShape[ii] == tmp )
        {
            m_PinShape->SetSelection( ii ); break ;
        }
    }

    tmp = CurrentPin ? CurrentPin->m_PinShape : LastPinShape;
    m_PinShape->SetSelection( 0 );
    for ( ii = 0; ii < NBSHAPES; ii++ )
    {
        if ( CodeShape[ii] == tmp )
        {
            m_PinShape->SetSelection( ii ); break ;
        }
    }

    m_PinElectricalType->SetSelection( CurrentPin ? CurrentPin->m_PinType : LastPinType);
}


/*!
 * wxEVT_SCROLL_LINEUP event handler for ID_SPINBUTTON_INC_DEC_PINSIZE
 */

void WinEDA_PinPropertiesFrame::OnSpinbuttonIncDecPinsizeUp( wxSpinEvent& event )
{
int pinsize;
wxString msg;

    // Get the new value, which can be edited by hand, so the value can not match the spin button value
    msg = m_PinSizeCtrl->GetValue();
    pinsize   = ReturnValueFromString( g_UnitMetric, msg, m_Parent->m_InternalUnits );
    pinsize += 10;
    if ( pinsize < 0 ) pinsize = 0;
    if ( pinsize > 2000 ) pinsize = 2000;
    msg = ReturnStringFromValue(g_UnitMetric, pinsize, m_Parent->m_InternalUnits);
    m_PinSizeCtrl->SetValue(msg);
    m_PinSizeIncDecButton->SetValue(pinsize);
}


/*!
 * wxEVT_SCROLL_LINEDOWN event handler for ID_SPINBUTTON_INC_DEC_PINSIZE
 */

void WinEDA_PinPropertiesFrame::OnSpinbuttonIncDecPinsizeDown( wxSpinEvent& event )
{
int pinsize;
wxString msg;

    // Get the new value, which can be edited by hand, so the value can not match the spin button value
    msg = m_PinSizeCtrl->GetValue();
    pinsize   = ReturnValueFromString( g_UnitMetric, msg, m_Parent->m_InternalUnits );
    pinsize -= 10;
    if ( pinsize < 0 ) pinsize = 0;
    if ( pinsize > 2000 ) pinsize = 2000;
    msg = ReturnStringFromValue(g_UnitMetric, pinsize, m_Parent->m_InternalUnits);
    m_PinSizeCtrl->SetValue(msg);
    m_PinSizeIncDecButton->SetValue(pinsize);
}

