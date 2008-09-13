/****************************************/
/* Modules to handle component drawing. */
/****************************************/

#include "fctsys.h"
#include "gr_basic.h"

#include "common.h"
#include "program.h"
#include "libcmp.h"
#include "component_class.h"
#include "general.h"
#include "trigo.h"
#include "protos.h"

#define UNVISIBLE_COLOR DARKGRAY

//#define DRAW_ARC_WITH_ANGLE		// Used to draw arcs


/* Fonctions locales */

/* Descr component <DUMMY> used when a component is not found in library,
 *  to draw a dummy shape*/

/*
 *  This component is a 400 mils square with the text ??
 *  DEF DUMMY U 0 40 Y Y 1 0 N
 *  F0 "U" 0 -350 60 H V
 *  F1 "DUMMY" 0 350 60 H V
 *  DRAW
 *  T 0 0 0 150 0 0 0 ??
 *  S -200 200 200 -200 0 1 0
 *  ENDDRAW
 *  ENDDEF
 */


static EDA_LibComponentStruct* DummyCmp;
static void DrawLibPartAux( WinEDA_DrawPanel * panel, wxDC * DC,
                            SCH_COMPONENT * Component,
                            EDA_LibComponentStruct * Entry,
                            const wxPoint &Pos,
                            int TransMat[2][2],
                            int Multi, int convert,
                            int DrawMode, int Color = -1, bool DrawPinText = TRUE );


/***************************************************************************/
wxPoint TransformCoordinate( int aTransformMatrix[2][2], wxPoint& aPosition )
/***************************************************************************/

/** Function TransformCoordinate
 * Calculate the wew coordinate from the old one, according to the transform matrix.
 * @param aTransformMatrix = rotation, mirror .. matrix
 * @param aPosition = the position to transform
 * @return the new coordinate
 */
{
    wxPoint new_pos;

    new_pos.x = (aTransformMatrix[0][0] * aPosition.x) + (aTransformMatrix[0][1] * aPosition.y);
    new_pos.y = (aTransformMatrix[1][0] * aPosition.x) + (aTransformMatrix[1][1] * aPosition.y);
    return new_pos;
}


/******************************/
static void CreateDummyCmp()
/******************************/
{
    DummyCmp = new              EDA_LibComponentStruct( NULL );

    LibDrawSquare* Square = new LibDrawSquare();

    Square->m_Pos   = wxPoint( -200, 200 );
    Square->m_End   = wxPoint( 200, -200 );
    Square->m_Width = 4;

    LibDrawText* Text = new LibDrawText();

    Text->m_Size.x = Text->m_Size.y = 150;
    Text->m_Text   = wxT( "??" );

    DummyCmp->m_Drawings = Square;
    Square->Pnext = Text;
}


/*************************************************************/
void DrawLibEntry( WinEDA_DrawPanel* panel, wxDC* DC,
                   EDA_LibComponentStruct* LibEntry,
                   int posX, int posY,
                   int Multi, int convert,
                   int DrawMode, int Color )
/**************************************************************/

/* Routine de dessin d'un composant d'une librairie
 *  LibEntry = pointeur sur la description en librairie
 *  posX, posY = position du composant
 *  DrawMode = GrOR ..
 *  Color = 0 : dessin en vraies couleurs, sinon couleur = Color
 *
 *  Une croix symbolise le point d'accrochage (ref position) du composant
 *
 *  Le composant est toujours trace avec orientation 0
 */
{
    int           color;
    int           TransMat[2][2];
    wxString      Prefix;
    LibDrawField* Field;
    wxPoint       text_pos;

    /* Orientation normale */
    TransMat[0][0] = 1; TransMat[1][1] = -1;
    TransMat[1][0] = TransMat[0][1] = 0;

    DrawLibPartAux( panel, DC, NULL, LibEntry, wxPoint( posX, posY ),
        TransMat, Multi,
        convert, DrawMode, Color );

    /* Trace des 2 champs ref et value (Attention aux coord: la matrice
     *  de transformation change de signe les coord Y */

    GRSetDrawMode( DC, DrawMode );

    if( LibEntry->m_Prefix.m_Attributs & TEXT_NO_VISIBLE )
    {
        if( Color >= 0 )
            color = Color;
        else
            color = UNVISIBLE_COLOR;
    }
    else
    {
        if( Color >= 0 )
            color = Color;
        else
            color = ReturnLayerColor( LAYER_REFERENCEPART );
    }

    if( LibEntry->m_UnitCount > 1 )
        Prefix.Printf( wxT( "%s?%c" ), LibEntry->m_Prefix.m_Text.GetData(), Multi + 'A' - 1 );
    else
        Prefix = LibEntry->m_Prefix.m_Text + wxT( "?" );

    text_pos.x = LibEntry->m_Prefix.m_Pos.x + posX;
    text_pos.y = posY - LibEntry->m_Prefix.m_Pos.y;
    int LineWidth = MAX( LibEntry->m_Prefix.m_Width, g_DrawMinimunLineWidth );
    if( (LibEntry->m_Prefix.m_Flags & IS_MOVED) == 0 )
        DrawGraphicText( panel, DC, text_pos,
            color, LibEntry->m_Prefix.m_Text.GetData(),
            LibEntry->m_Prefix.m_Orient ? TEXT_ORIENT_VERT : TEXT_ORIENT_HORIZ,
            LibEntry->m_Prefix.m_Size,
            LibEntry->m_Prefix.m_HJustify, LibEntry->m_Prefix.m_VJustify, LineWidth );

    if( LibEntry->m_Name.m_Attributs & TEXT_NO_VISIBLE )
    {
        if( Color >= 0 )
            color = Color;
        else
            color = UNVISIBLE_COLOR;
    }
    else
    {
        if( Color >= 0 )
            color = Color;
        else
            color = ReturnLayerColor( LAYER_VALUEPART );
    }

    text_pos.x = LibEntry->m_Name.m_Pos.x + posX;
    text_pos.y = posY - LibEntry->m_Name.m_Pos.y;
    LineWidth  = MAX( LibEntry->m_Name.m_Width, g_DrawMinimunLineWidth );
    if( (LibEntry->m_Name.m_Flags & IS_MOVED) == 0 )
        DrawGraphicText( panel, DC, text_pos,
            color, LibEntry->m_Name.m_Text.GetData(),
            LibEntry->m_Name.m_Orient ? TEXT_ORIENT_VERT : TEXT_ORIENT_HORIZ,
            LibEntry->m_Name.m_Size,
            LibEntry->m_Name.m_HJustify, LibEntry->m_Name.m_VJustify, LineWidth );

    for( Field = LibEntry->Fields; Field != NULL; Field = (LibDrawField*) Field->Pnext )
    {
        if( Field->m_Text.IsEmpty() )
            return;
        if( (Field->m_Flags & IS_MOVED) != 0 )
            continue;
        if( Field->m_Attributs & TEXT_NO_VISIBLE )
        {
            if( Color >= 0 )
                color = Color;
            else
                color = UNVISIBLE_COLOR;
        }
        else
        {
            if( Color >= 0 )
                color = Color;
            else
                color = ReturnLayerColor( LAYER_FIELDS );
        }

        text_pos.x = Field->m_Pos.x + posX;
        text_pos.y = posY - Field->m_Pos.y;
        LineWidth  = MAX( Field->m_Width, g_DrawMinimunLineWidth );
        DrawGraphicText( panel, DC, text_pos,
            color, Field->m_Text,
            Field->m_Orient ? TEXT_ORIENT_VERT : TEXT_ORIENT_HORIZ,
            Field->m_Size,
            Field->m_HJustify, Field->m_VJustify, LineWidth );
    }

    // Trac� de l'ancre
    int len = 3 * panel->GetZoom();
    GRLine( &panel->m_ClipBox, DC, posX, posY - len, posX, posY + len, 0, color );
    GRLine( &panel->m_ClipBox, DC, posX - len, posY, posX + len, posY, 0, color );
}


/*****************************************************************************
* Routine to draw the given part at given position, transformed/mirror as	 *
* specified, and in the given drawing mode. Only this one is visible...		 *
*****************************************************************************/
void SCH_COMPONENT::Draw( WinEDA_DrawPanel* panel, wxDC* DC,
                          const wxPoint& offset, int DrawMode, int Color )
{
    EDA_LibComponentStruct* Entry;
    int  ii;
    bool dummy = FALSE;

    if( ( Entry = FindLibPart( m_ChipName.GetData(), wxEmptyString, FIND_ROOT ) ) == NULL )
    {   /* composant non trouve, on affiche un composant "dummy" */
        dummy = TRUE;
        if( DummyCmp == NULL )
            CreateDummyCmp();
        Entry = DummyCmp;
    }

    DrawLibPartAux( panel, DC, this, Entry, m_Pos + offset,
        m_Transform,
        dummy ? 0 : m_Multi,
        dummy ? 0 : m_Convert,
        DrawMode );

    /* Trace des champs, avec placement et orientation selon orient. du
     *  composant
     */

    if( ( (m_Field[REFERENCE].m_Attributs & TEXT_NO_VISIBLE) == 0 )
       && !(m_Field[REFERENCE].m_Flags & IS_MOVED) )
    {
        if( Entry->m_UnitCount > 1 )
        {
            m_Field[REFERENCE].m_AddExtraText = true;
            m_Field[REFERENCE].Draw( panel, DC, offset, DrawMode );
        }
        else
        {
            m_Field[REFERENCE].m_AddExtraText = false;
            m_Field[REFERENCE].Draw( panel, DC, offset, DrawMode );
        }
    }

    for( ii = VALUE; ii < NUMBER_OF_FIELDS; ii++ )
    {
        if( m_Field[ii].m_Flags & IS_MOVED )
            continue;
        m_Field[ii].Draw( panel, DC, offset, DrawMode );
    }
}


/***********************************************************/
void PartTextStruct::Draw( WinEDA_DrawPanel* panel,
                           wxDC*             DC,
                           const wxPoint&    offset,
                           int               DrawMode,
                           int               Color )
/***********************************************************/

/* Routine de trace des textes type Field du composant.
 *  entree:
 *      DrawMode: mode de trace
 */
{
    int            orient, color;

    wxPoint        pos; /* Position des textes */
    SCH_COMPONENT* DrawLibItem = (SCH_COMPONENT*) m_Parent;
    int            hjustify, vjustify;
    int            LineWidth = MAX( m_Width, g_DrawMinimunLineWidth );

    if( m_Attributs & TEXT_NO_VISIBLE )
        return;
    if( IsVoid() )
        return;

    GRSetDrawMode( DC, DrawMode );

    /* Calcul de la position des textes, selon orientation du composant */
    orient   = m_Orient;
    hjustify = m_HJustify; vjustify = m_VJustify;
    pos = m_Pos - DrawLibItem->m_Pos;

    pos  = DrawLibItem->GetScreenCoord( pos );
    pos += DrawLibItem->m_Pos;

    /* Y a t-il rotation (pour l'orientation, la justification)*/
    if( DrawLibItem->m_Transform[0][1] )  // Rotation du composant de 90deg
    {
        if( orient == TEXT_ORIENT_HORIZ )
            orient = TEXT_ORIENT_VERT;
        else
            orient = TEXT_ORIENT_HORIZ;
        /* Y a t-il rotation, miroir (pour les justifications)*/
        EXCHG( hjustify, vjustify );
        if( DrawLibItem->m_Transform[1][0] < 0 )
            vjustify = -vjustify;
        if( DrawLibItem->m_Transform[1][0] > 0 )
            hjustify = -hjustify;
    }
    else
    {
        /* Texte horizontal: Y a t-il miroir (pour les justifications)*/
        if( DrawLibItem->m_Transform[0][0] < 0 )
            hjustify = -hjustify;
        if( DrawLibItem->m_Transform[1][1] > 0 )
            vjustify = -vjustify;
    }

    if( m_FieldId == REFERENCE )
        color = ReturnLayerColor( LAYER_REFERENCEPART );
    else if( m_FieldId == VALUE )
        color = ReturnLayerColor( LAYER_VALUEPART );
    else
        color = ReturnLayerColor( LAYER_FIELDS );
    if( !m_AddExtraText || (m_FieldId != REFERENCE) )
    {
        DrawGraphicText( panel, DC, pos, color, m_Text.GetData(),
            orient ? TEXT_ORIENT_VERT : TEXT_ORIENT_HORIZ,
            m_Size,
            hjustify, vjustify, LineWidth );
    }
    else // Si il y a plusieurs parts par boitier, ajouter a la reference l'identification de la selection ( A, B ... )
    {
        /* On ajoute alors A ou B ... a la reference */
        wxString fulltext = m_Text;
        fulltext.Append( 'A' - 1 + DrawLibItem->m_Multi );
        DrawGraphicText( panel, DC, pos, color, fulltext.GetData(),
            orient ? TEXT_ORIENT_VERT : TEXT_ORIENT_HORIZ,
            m_Size,
            hjustify, vjustify, LineWidth );
    }
}


/********************************************************************************/
EDA_LibComponentStruct* FindLibPart( const wxChar* Name, const wxString& LibName, int Alias )
/********************************************************************************/

/*
 *  Routine to find a part in one of the libraries given its name.
 *  Name = Name of part.
 *  LibName = Name of Lib; if "": seach in all libs
 *  Alias = Flag: si flag != 0, retourne un pointeur sur une part ou un alias
 *                si flag = 0, retourne un pointeur sur une part meme si le nom
 *                correspond a un alias
 *  Alias = FIND_ROOT, ou Alias = FIND_ALIAS
 */
{
    EDA_LibComponentStruct*       Entry;

    static EDA_LibComponentStruct DummyEntry( wxEmptyString );  /* Used only to call PQFind. */

    LibraryStruct* Lib = g_LibraryList;

    DummyEntry.m_Drawings    = NULL; /* Used only to call PQFind. */
    DummyEntry.m_Name.m_Text = Name;

    PQCompFunc( (PQCompFuncType) LibraryEntryCompare );

    Entry = NULL;
    FindLibName.Empty();

    while( Lib )
    {
        if( !LibName.IsEmpty() )
        {
            if( Lib->m_Name != LibName )
            {
                Lib = Lib->m_Pnext;
                continue;
            }
        }

        if( Lib == NULL )
            break;

        Entry = (EDA_LibComponentStruct*) PQFind( Lib->m_Entries, &DummyEntry );
        if( Entry != NULL )
        {
            FindLibName = Lib->m_Name;
            break;
        }

        Lib = Lib->m_Pnext;
    }

    /* Si le nom est un alias, recherche du vrai composant */
    if( Entry )
    {
        if( (Entry->Type != ROOT ) && (Alias == FIND_ROOT) )
            Entry = FindLibPart( ( (EDA_LibCmpAliasStruct*) Entry )->m_RootName.GetData(),
                Lib->m_Name, FIND_ROOT );
    }

    return Entry;
}


/*****************************************************************************
* Routine to draw the given part at given position, transformed/mirror as
* specified, and in the given drawing mode.
* if Color < 0: Draw in normal color
* else draw  in color = Color
*****************************************************************************/
/* DrawMode  = GrXOR, GrOR ..*/
void DrawLibPartAux( WinEDA_DrawPanel* panel, wxDC* DC,
                     SCH_COMPONENT* Component,
                     EDA_LibComponentStruct* Entry,
                     const wxPoint& Pos,
                     int TransMat[2][2],
                     int Multi, int convert, int DrawMode,
                     int Color, bool DrawPinText )
{
    wxPoint            pos1, pos2;
    LibEDA_BaseStruct* DEntry = NULL;
    bool               force_nofill;
    int                SetHightColor;
    int                LineWidth;

    if( Entry->m_Drawings == NULL )
        return;
    GRSetDrawMode( DC, DrawMode );

    for( DEntry = Entry->m_Drawings; DEntry != NULL; DEntry = DEntry->Next() )
    {
        /* Elimination des elements non relatifs a l'unite */
        if( Multi && DEntry->m_Unit && (DEntry->m_Unit != Multi) )
            continue;

        if( convert && DEntry->m_Convert && (DEntry->m_Convert != convert) )
            continue;

        if( DEntry->m_Flags & IS_MOVED )
            continue; // Element en deplacement non trace

        SetHightColor = (DEntry->m_Selected & IS_SELECTED) ? HIGHT_LIGHT_FLAG : 0;
        LineWidth = MAX( DEntry->m_Width, g_DrawMinimunLineWidth );

        switch( DEntry->Type() )
        {
        case COMPONENT_ARC_DRAW_TYPE:
        {
            LibDrawArc* Arc = (LibDrawArc*) DEntry;
            force_nofill = false;
            if( g_IsPrinting && Arc->m_Fill == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
                force_nofill = true;
            DEntry->Draw( panel, DC, Pos, Color, DrawMode, (void*)force_nofill, TransMat );
            break;
        }
        case COMPONENT_CIRCLE_DRAW_TYPE:
        {
            LibDrawCircle* Circle = (LibDrawCircle*) DEntry;
            force_nofill = false;
            if( g_IsPrinting && Circle->m_Fill == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
                force_nofill = true;
            DEntry->Draw( panel, DC, Pos, Color, DrawMode, (void*)force_nofill, TransMat );
        }
            break;

        case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
            DEntry->Draw( panel, DC, Pos, Color, DrawMode, NULL, TransMat );
            break;

        case COMPONENT_RECT_DRAW_TYPE:
        {
            LibDrawSquare* Square = (LibDrawSquare*) DEntry;
            force_nofill = false;
            if( g_IsPrinting && Square->m_Fill == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
                force_nofill = true;
            DEntry->Draw( panel, DC, Pos, Color, DrawMode, (void*)force_nofill, TransMat );
        }
            break;

        case COMPONENT_PIN_DRAW_TYPE:     /* Trace des Pins */
        {
            DrawPinPrms prms(Entry, DrawPinText);
            DEntry->Draw( panel, DC, Pos, Color, DrawMode, &prms, TransMat );
        }
            break;

        case COMPONENT_POLYLINE_DRAW_TYPE:
        {
            LibDrawPolyline* polyline = (LibDrawPolyline*) DEntry;
            force_nofill = false;
            if( g_IsPrinting && polyline->m_Fill == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
                force_nofill = true;
            DEntry->Draw( panel, DC, Pos, Color, DrawMode, (void*)force_nofill, TransMat );
        }
            break;

        default:
            wxBell();
            break;
        }

        /* Fin Switch */
    }

    /* Fin Boucle de dessin */
    if( g_DebugLevel > 4 ) /* Draw the component boundary box */
    {
        EDA_Rect BoundaryBox;
        if( Component )
            BoundaryBox = Component->GetBoundaryBox();
        else
            BoundaryBox = Entry->GetBoundaryBox( Multi, convert );
        int x1 = BoundaryBox.GetX();
        int y1 = BoundaryBox.GetY();
        int x2 = BoundaryBox.GetRight();
        int y2 = BoundaryBox.GetBottom();
        GRRect( &panel->m_ClipBox, DC, x1, y1, x2, y2, BROWN );
        BoundaryBox = Component->m_Field[REFERENCE].GetBoundaryBox();
        x1 = BoundaryBox.GetX();
        y1 = BoundaryBox.GetY();
        x2 = BoundaryBox.GetRight();
        y2 = BoundaryBox.GetBottom();
        GRRect( &panel->m_ClipBox, DC, x1, y1, x2, y2, BROWN );
        BoundaryBox = Component->m_Field[VALUE].GetBoundaryBox();
        x1 = BoundaryBox.GetX();
        y1 = BoundaryBox.GetY();
        x2 = BoundaryBox.GetRight();
        y2 = BoundaryBox.GetBottom();
        GRRect( &panel->m_ClipBox, DC, x1, y1, x2, y2, BROWN );
    }
}



/*****************************************************************************
* Routine to rotate the given angular direction by the given Transformation. *
* Input (and output) angles must be as follows:								 *
*	Unit is 0.1 degre														 *
* Angle1 in [0..3600], Angle2 > Angle1 in [0..7200]. Arc is assumed to be less *
* than 180.0 degrees.															 *
* Algorithm:																 *
* Map the angles to a point on the unit circle which is mapped using the	 *
* transform (only mirror and rotate so it remains on the unit circle) to	 *
* a new point which is used to detect new angle.							 *
*****************************************************************************/
bool MapAngles( int* Angle1, int* Angle2, int TransMat[2][2] )
{
    int    Angle, Delta;
    double x, y, t;
    bool   swap = FALSE;

    Delta = *Angle2 - *Angle1;
    if( Delta >= 1800 )
    {
        *Angle1 -= 1;
        *Angle2 += 1;
    }

    x = cos( *Angle1 * M_PI / 1800.0 );
    y = sin( *Angle1 * M_PI / 1800.0 );
    t = x * TransMat[0][0] + y * TransMat[0][1];
    y = x * TransMat[1][0] + y * TransMat[1][1];
    x = t;
    *Angle1 = (int) (atan2( y, x ) * 1800.0 / M_PI + 0.5);

    x = cos( *Angle2 * M_PI / 1800.0 );
    y = sin( *Angle2 * M_PI / 1800.0 );
    t = x * TransMat[0][0] + y * TransMat[0][1];
    y = x * TransMat[1][0] + y * TransMat[1][1];
    x = t;
    *Angle2 = (int) (atan2( y, x ) * 1800.0 / M_PI + 0.5);

    NORMALIZE_ANGLE( *Angle1 );
    NORMALIZE_ANGLE( *Angle2 );
    if( *Angle2 < *Angle1 )
        *Angle2 += 3600;

    if( *Angle2 - *Angle1 > 1800 )
    {                /* Need to swap the two angles. */
        Angle   = (*Angle1);
        *Angle1 = (*Angle2);
        *Angle2 = Angle;

        NORMALIZE_ANGLE( *Angle1 );
        NORMALIZE_ANGLE( *Angle2 );
        if( *Angle2 < *Angle1 )
            *Angle2 += 3600;
        swap = TRUE;
    }

    if( Delta >= 1800 )
    {
        *Angle1 += 1;
        *Angle2 -= 1;
    }

    return swap;
}


/*****************************************************************************
* Routine to display an outline version of given library entry.				 *
* This routine is applied by the PlaceLibItem routine above.			 	 *
*****************************************************************************/
void DrawingLibInGhost( WinEDA_DrawPanel* panel, wxDC* DC,
                        EDA_LibComponentStruct* LibEntry,
                        SCH_COMPONENT* DrawLibItem, int PartX, int PartY,
                        int multi, int convert, int Color, bool DrawPinText )
{
    int DrawMode = g_XorMode;

    DrawLibPartAux( panel, DC, DrawLibItem, LibEntry, wxPoint( PartX, PartY ),
        DrawLibItem->m_Transform,
        multi, convert, DrawMode, Color, DrawPinText );
}


/************************************************************/
/* Routine to draw One LibraryDrawStruct at given position, */
/* matrice de transformation  1 0 0 -1 (normale)			*/
/* DrawMode  = GrXOR, GrOR ..								*/
/************************************************************/
/* Utilise en LibEdit et Lib Browse */
void DrawLibraryDrawStruct( WinEDA_DrawPanel* panel, wxDC* DC,
                            EDA_LibComponentStruct* LibEntry,
                            int PartX, int PartY,
                            LibEDA_BaseStruct* DrawItem, int Multi,
                            int DrawMode, int Color )
{
    int x1, y1, x2, y2, t1, t2, orient;
    int CharColor;
    int TransMat[2][2];
    int fill_option;
    wxPoint position(PartX, PartY);

#undef GETCOLOR
#define GETCOLOR( l ) Color < 0 ? ReturnLayerColor( l ) : Color;

    Multi = 0;  /* unused */
    /* Trace de la structure */
    CharColor = GETCOLOR( LAYER_DEVICE );
    GRSetDrawMode( DC, DrawMode );

    TransMat[0][0] = 1;
    TransMat[0][1] = TransMat[1][0] = 0;
    TransMat[1][1] = -1;

    int LineWidth = MAX( DrawItem->m_Width, g_DrawMinimunLineWidth );

    switch( DrawItem->Type() )
    {
    case COMPONENT_ARC_DRAW_TYPE:
    {
        int         xc, yc, x2, y2;
        LibDrawArc* Arc = (LibDrawArc*) DrawItem;
        t1 = Arc->t1; t2 = Arc->t2;
        bool        swap = MapAngles( &t1, &t2, TransMat );
        xc = PartX + Arc->m_Pos.x;
        yc = PartY - Arc->m_Pos.y;
        x2 = PartX + Arc->m_ArcStart.x;
        y2 = PartY - Arc->m_ArcStart.y;
        x1 = PartX + Arc->m_ArcEnd.x;
        y1 = PartY - Arc->m_ArcEnd.y;

        if( swap )
        {
            EXCHG( x1, x2 ); EXCHG( y1, y2 )
        }
        fill_option = Arc->m_Fill;
        if( g_IsPrinting && fill_option == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
            fill_option = NO_FILL;
        if( fill_option == FILLED_WITH_BG_BODYCOLOR )
            GRFilledArc( &panel->m_ClipBox, DC, xc, yc, t1, t2,
                Arc->m_Rayon, CharColor,
                ReturnLayerColor( LAYER_DEVICE_BACKGROUND ) );
        else if( fill_option == FILLED_SHAPE )
            GRFilledArc( &panel->m_ClipBox, DC, xc, yc, t1, t2,
                Arc->m_Rayon, LineWidth, CharColor, CharColor );
#ifdef DRAW_ARC_WITH_ANGLE
        else
            GRArc( &panel->m_ClipBox, DC, xc, yc, t1, t2,
                Arc->m_Rayon, CharColor );
#else
        else
            GRArc1( &panel->m_ClipBox, DC, x1, y1, x2, y2,
                xc, yc, LineWidth, CharColor );
#endif
    }
        break;

    case COMPONENT_CIRCLE_DRAW_TYPE:
    {
        LibDrawCircle* Circle = (LibDrawCircle*) DrawItem;
        x1 = PartX + Circle->m_Pos.x;
        y1 = PartY - Circle->m_Pos.y;
        fill_option = Circle->m_Fill;
        if( g_IsPrinting && fill_option == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
            fill_option = NO_FILL;
        if( fill_option == FILLED_WITH_BG_BODYCOLOR )
            GRFilledCircle( &panel->m_ClipBox, DC, x1, y1,
                Circle->m_Rayon, LineWidth, CharColor,
                ReturnLayerColor( LAYER_DEVICE_BACKGROUND ) );
        else if( fill_option == FILLED_SHAPE )
            GRFilledCircle( &panel->m_ClipBox, DC, x1, y1,
                Circle->m_Rayon, 0, CharColor, CharColor );
        else
            GRCircle( &panel->m_ClipBox, DC, x1, y1,
                Circle->m_Rayon, LineWidth, CharColor );
    }
        break;

    case COMPONENT_GRAPHIC_TEXT_DRAW_TYPE:
    {
        LibDrawText* Text = (LibDrawText*) DrawItem;
        x1 = PartX + Text->m_Pos.x;
        y1 = PartY - Text->m_Pos.y;
        DrawGraphicText( panel, DC, wxPoint( x1, y1 ), CharColor, Text->m_Text,
            Text->m_Horiz,
            Text->m_Size,
            GR_TEXT_HJUSTIFY_CENTER, GR_TEXT_VJUSTIFY_CENTER, LineWidth );
    }
        break;

    case COMPONENT_RECT_DRAW_TYPE:
    {
        LibDrawSquare* Square = (LibDrawSquare*) DrawItem;
        x1 = PartX + Square->m_Pos.x;
        y1 = PartY - Square->m_Pos.y;
        x2 = PartX + Square->m_End.x;
        y2 = PartY - Square->m_End.y;
        fill_option = Square->m_Fill;
        if( g_IsPrinting && fill_option == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
            fill_option = NO_FILL;
        if( fill_option == FILLED_WITH_BG_BODYCOLOR )
            GRFilledRect( &panel->m_ClipBox, DC, x1, y1, x2, y2,
                CharColor, LineWidth,
                ReturnLayerColor( LAYER_DEVICE_BACKGROUND ) );
        else if( fill_option == FILLED_SHAPE )
            GRFilledRect( &panel->m_ClipBox, DC, x1, y1, x2, y2,
                CharColor, CharColor );
        else
            GRRect( &panel->m_ClipBox, DC, x1, y1, x2, y2, LineWidth,
                CharColor );
    }
        break;

    case COMPONENT_PIN_DRAW_TYPE:     /* Trace des Pins */
    {
        LibDrawPin* Pin = (LibDrawPin*) DrawItem;
        x2 = PartX + Pin->m_Pos.x;
        y2 = PartY - Pin->m_Pos.y;
        /* Compute the real pin orientation, i.e. pin orient + component orient */
        orient = Pin->ReturnPinDrawOrient( TransMat );

        /* Dessin de la pin et du symbole special associe */
        if( Pin->m_Attributs & PINNOTDRAW )
            CharColor = DARKGRAY;
        else
            CharColor = -1;

        Pin->DrawPinSymbol( panel, DC, wxPoint( x2, y2 ), orient, DrawMode );
        wxPoint pinpos( x2, y2 );

        Pin->DrawPinTexts( panel, DC, pinpos, orient,
            LibEntry->m_TextInside,
            LibEntry->m_DrawPinNum, LibEntry->m_DrawPinName,
            CharColor, DrawMode );
    }
        break;

    case COMPONENT_POLYLINE_DRAW_TYPE:
    {
        LibDrawPolyline* polyline = (LibDrawPolyline*) DrawItem;
        fill_option = false;
        if( g_IsPrinting && polyline->m_Fill == FILLED_WITH_BG_BODYCOLOR && GetGRForceBlackPenState() )
            fill_option = true;
        DrawItem->Draw( panel, DC, position, Color,DrawMode, (void *) fill_option, TransMat );
        break;
    }

    default:
        ;
    }
}
