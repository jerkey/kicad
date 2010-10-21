/* read_from_file_schematic_items_descriptions.cpp */

/*functions to read schematic items descriptions from file
 */
#include "fctsys.h"
#include "common.h"
#include "confirm.h"
#include "kicad_string.h"
#include "drawtxt.h"

#include "program.h"
#include "general.h"
#include "protos.h"
#include "richio.h"

#define line         ((char*)(*aLine))

SCH_ITEM* ReadTextDescr( LINE_READER* aLine, wxString& aMsgDiag, int aSchematicFileVersion )
{
/**
 * Function ReadTextDescr
 * Reads the data structures for a Text (Comment, label, Hlabel and Hlabel
 * from a FILE in "*.sch" format.
 * @param aLine is a LINE_READER to use.
 * @return a pointer to the new created object if success reading else NULL.
 */
    SCH_ITEM* Struct = NULL;
    char      Name1[256];
    char      Name2[256];
    char      Name3[256];
    int       thickness = 0, size = 0, orient = 0;
    wxPoint   pos;


    Name1[0] = 0; Name2[0] = 0; Name3[0] = 0;

    char*     sline = line;
    while( (*sline != ' ' ) && *sline )
        sline++;

    // sline points the start of parameters
    int ii = sscanf( sline, "%s %d %d %d %d %s %s %d", Name1, &pos.x, &pos.y,
                     &orient, &size, Name2, Name3, &thickness );

    if( ii < 4 )
    {
        aMsgDiag.Printf(
            wxT( "EESchema file text struct error line %d, aborted" ),
            aLine->LineNumber() );
        return NULL;
    }

    if( !aLine->ReadLine() )
    {
        aMsgDiag.Printf(
            wxT( "EESchema file text struct error line %d (No text), aborted" ),
            aLine->LineNumber() );
        return NULL;
    }

   if( size == 0 )
        size = DEFAULT_SIZE_TEXT;

    char* text = strtok( line, "\n\r" );
    if( text == NULL )
        return NULL;

    if( Name1[0] == 'L' )  // Reading a simple label (SCH_LABEL item)
    {
        SCH_LABEL* TextStruct = new SCH_LABEL( pos, CONV_FROM_UTF8( text ) );

        TextStruct->m_Size.x = TextStruct->m_Size.y = size;
        TextStruct->SetSchematicTextOrientation( orient );
        if( isdigit( Name3[0] ) )
        {
            thickness = atol( Name3 );
            TextStruct->m_Bold = (thickness != 0);
            TextStruct->m_Width =
                TextStruct->m_Bold ? GetPenSizeForBold( size ) : 0;
        }
        Struct = TextStruct;
        if( stricmp( Name2, "Italic" ) == 0 )
            TextStruct->m_Italic = 1;
    }
    else if( Name1[0] == 'G' && aSchematicFileVersion > '1' )
    {
        // Reading a global label (SCH_GLOBALLABEL item).
        SCH_GLOBALLABEL* TextStruct =
            new SCH_GLOBALLABEL( pos, CONV_FROM_UTF8( text ) );

        Struct = TextStruct;
        TextStruct->m_Size.x = TextStruct->m_Size.y = size;
        TextStruct->SetSchematicTextOrientation(  orient );
        TextStruct->m_Shape  = NET_INPUT;
        TextStruct->m_Bold = (thickness != 0);
        TextStruct->m_Width =
            TextStruct->m_Bold ? GetPenSizeForBold( size ) : 0;

        if( stricmp( Name2, SheetLabelType[NET_OUTPUT] ) == 0 )
            TextStruct->m_Shape = NET_OUTPUT;
        if( stricmp( Name2, SheetLabelType[NET_BIDI] ) == 0 )
            TextStruct->m_Shape = NET_BIDI;
        if( stricmp( Name2, SheetLabelType[NET_TRISTATE] ) == 0 )
            TextStruct->m_Shape = NET_TRISTATE;
        if( stricmp( Name2, SheetLabelType[NET_UNSPECIFIED] ) == 0 )
            TextStruct->m_Shape = NET_UNSPECIFIED;
        if( stricmp( Name3, "Italic" ) == 0 )
            TextStruct->m_Italic = 1;
    }
    else if( (Name1[0] == 'H')
            || (Name1[0] == 'G' && aSchematicFileVersion == '1') )
    {
        // Reading a hierarchical label (SCH_HIERLABEL item).
        // In schematic file version 1, glabels were actually hierarchical
        // labels.
        SCH_HIERLABEL* TextStruct = new SCH_HIERLABEL( pos,
                                                       CONV_FROM_UTF8( text ) );

        Struct = TextStruct;
        TextStruct->m_Size.x = TextStruct->m_Size.y = size;
        TextStruct->SetSchematicTextOrientation( orient );
        TextStruct->m_Shape = NET_INPUT;
        TextStruct->m_Bold = (thickness != 0);
        TextStruct->m_Width =
            TextStruct->m_Bold ? GetPenSizeForBold( size ) : 0;

        if( stricmp( Name2, SheetLabelType[NET_OUTPUT] ) == 0 )
            TextStruct->m_Shape = NET_OUTPUT;
        if( stricmp( Name2, SheetLabelType[NET_BIDI] ) == 0 )
            TextStruct->m_Shape = NET_BIDI;
        if( stricmp( Name2, SheetLabelType[NET_TRISTATE] ) == 0 )
            TextStruct->m_Shape = NET_TRISTATE;
        if( stricmp( Name2, SheetLabelType[NET_UNSPECIFIED] ) == 0 )
            TextStruct->m_Shape = NET_UNSPECIFIED;
        if( stricmp( Name3, "Italic" ) == 0 )
            TextStruct->m_Italic = 1;
    }
    else        // reading a graphic text (comment)
    {
        wxString val = CONV_FROM_UTF8( text );

        for( ;; )
        {
            int i=val.find( wxT( "\\n" ) );

            if( i == wxNOT_FOUND )
                break;
            val.erase( i, 2 );
            val.insert( i, wxT( "\n" ) );
        }

        SCH_TEXT* TextStruct = new SCH_TEXT( pos, val );

        TextStruct->m_Size.x = TextStruct->m_Size.y = size;
        TextStruct->SetSchematicTextOrientation( orient );

        if( isdigit( Name3[0] ) )
        {
            thickness = atol( Name3 );
            TextStruct->m_Bold = (thickness != 0);
            TextStruct->m_Width =
                TextStruct->m_Bold ? GetPenSizeForBold( size ) : 0;
        }

        if( strnicmp( Name2, "Italic", 6 ) == 0 )
            TextStruct->m_Italic = 1;

        Struct = TextStruct;
    }

    return Struct;
}


/* Function used by LoadEEFile().
 * Get the lines for a description of a piece of hierarchy.
 */
int ReadSheetDescr( wxWindow* frame, LINE_READER* aLine, wxString& aMsgDiag, BASE_SCREEN* Window )
{
    int              ii, fieldNdx, size;
    char             Name1[256], Char1[256], Char2[256];
    SCH_SHEET*       SheetStruct;
    SCH_SHEET_PIN*   SheetLabelStruct;
    int              Failed = FALSE;
    char*            ptcar;

    SheetStruct = new SCH_SHEET();

    SheetStruct->m_TimeStamp = GetTimeStamp();

    // sheets are added to the EEDrawList like other schematic components.
    // however, in order to preserve the hierarchy (through m_Parent pointers),
    // a duplicate of the sheet is added to m_SubSheet array.
    // must be a duplicate, references just work for a two-layer structure.
    // this is accomplished through the Sync() function.

    if( line[0] == '$' )   // line should be "$Sheet"
    {
        if( !aLine->ReadLine() )
        {
            aMsgDiag.Printf( wxT( "Read File Errror" ) );
            return TRUE;
        }
    }

    /* Next line: must be "S xx yy nn mm" with xx, yy = sheet position
     *  ( upper left corner  ) et nn,mm = sheet size */
    if( (sscanf( &line[1], "%d %d %d %d",
                 &SheetStruct->m_Pos.x, &SheetStruct->m_Pos.y,
                 &SheetStruct->m_Size.x, &SheetStruct->m_Size.y ) != 4)
       || (line[0] != 'S' ) )
    {
        aMsgDiag.Printf(
            wxT( " ** EESchema file sheet struct error at line %d, aborted\n" ),
            aLine->LineNumber() );

        aMsgDiag << CONV_FROM_UTF8( line );
        Failed = TRUE;
        return Failed;
    }

    /* Read fields */
    for( ; ; ) /* Analysis of lines "Fn" text. */
    {
        if( !aLine->ReadLine() )
            return TRUE;

        if( line[0] == 'U' )
        {
            sscanf( line + 1, "%lX", &SheetStruct->m_TimeStamp );
            if( SheetStruct->m_TimeStamp == 0 )  // zero is not unique!
                SheetStruct->m_TimeStamp = GetTimeStamp();
            continue;
        }

        if( line[0] != 'F' )
            break;

        sscanf( line + 1, "%d", &fieldNdx );


        /* Read the field:
         * If fieldNdx> = 2: Fn "text" t s posx posy
         * If F0 "text" for SheetName
         * F1 and "text" for filename
         */
        ptcar = line;
        while( *ptcar && ( *ptcar != '"' ) )
            ptcar++;

        if( *ptcar != '"' )
        {
            aMsgDiag.Printf(
                wxT( "EESchema file sheet label F%d at line %d, aborted\n" ),
                fieldNdx, aLine->LineNumber() );
            aMsgDiag << CONV_FROM_UTF8( line );
            return TRUE;
        }

        for( ptcar++, ii = 0; ; ii++, ptcar++ )
        {
            Name1[ii] = *ptcar;

            if( *ptcar == 0 )
            {
                aMsgDiag.Printf(
                    wxT( "EESchema file sheet field F at line %d, aborted\n" ),
                    aLine->LineNumber() );
                aMsgDiag << CONV_FROM_UTF8( line );
                return TRUE;
            }

            if( *ptcar == '"' )
            {
                Name1[ii] = 0;
                ptcar++;
                break;
            }
        }

        if( ( fieldNdx == 0 ) || ( fieldNdx == 1 ) )
        {
            if( sscanf( ptcar, "%d", &size ) != 1 )
            {
                aMsgDiag.Printf( wxT( "EESchema file sheet Label Caract error line %d, aborted\n" ),
                    aLine->LineNumber() );

                aMsgDiag << CONV_FROM_UTF8( line );
                DisplayError( frame, aMsgDiag );
            }
            if( size == 0 )
                size = DEFAULT_SIZE_TEXT;

            if( fieldNdx == 0 )
            {
                SheetStruct->m_SheetName     = CONV_FROM_UTF8( Name1 );
                SheetStruct->m_SheetNameSize = size;
            }
            else
            {
                SheetStruct->SetFileName( CONV_FROM_UTF8( Name1 ) );

                //printf( "in ReadSheetDescr : SheetStruct->m_FileName = %s \n",
                //        Name1 );
                SheetStruct->m_FileNameSize = size;
            }
        }

        if( fieldNdx > 1 )
        {
            int x, y;

            /* Read coordinates. */
            if( sscanf( ptcar, "%s %s %d %d %d", Char1, Char2, &x, &y, &size ) != 5 )
            {
                aMsgDiag.Printf( wxT( "EESchema file sheet label error at line %d, ignoring.\n" ),
                                 aLine->LineNumber() );
                aMsgDiag << CONV_FROM_UTF8( line );
                DisplayError( frame, aMsgDiag );
                continue;
            }

            SheetLabelStruct = new SCH_SHEET_PIN( SheetStruct, wxPoint( x, y ),
                                                  CONV_FROM_UTF8( Name1 ) );

            if( size == 0 )
                size = DEFAULT_SIZE_TEXT;

            SheetLabelStruct->m_Size.x = SheetLabelStruct->m_Size.y = size;
            SheetLabelStruct->m_Pos.x=x; //to readjust x of first label if vertical
            switch( Char1[0] )
            {
            case 'I':
                SheetLabelStruct->m_Shape = NET_INPUT;
                break;

            case 'O':
                SheetLabelStruct->m_Shape = NET_OUTPUT;
                break;

            case 'B':
                SheetLabelStruct->m_Shape = NET_BIDI;
                break;

            case 'T':
                SheetLabelStruct->m_Shape = NET_TRISTATE;
                break;

            case 'U':
                SheetLabelStruct->m_Shape = NET_UNSPECIFIED;
                break;
            }

            switch( Char2[0] )
            {
            case 'R' : /* pin on right side */
                SheetLabelStruct->SetEdge(1);
                break;
            case 'T' : /* pin on top side */
                SheetLabelStruct->SetEdge(2);
                break;
            case 'B' : /* pin on bottom side */
                SheetLabelStruct->SetEdge(3);
                break;
            case 'L' : /* pin on left side */
            default  :
                SheetLabelStruct->SetEdge(0);
                break;
            }
            SheetStruct->AddLabel( SheetLabelStruct );
        }
    }

    if( strnicmp( "$End", line, 4 ) != 0 )
    {
        aMsgDiag.Printf( wxT( "**EESchema file end_sheet struct error at line %d, aborted\n" ),
                         aLine->LineNumber() );
        aMsgDiag << CONV_FROM_UTF8( line );
        Failed = TRUE;
    }

    if( !Failed )
    {
        SheetStruct->SetNext( Window->EEDrawList );
        Window->EEDrawList = SheetStruct;
        SheetStruct->SetParent( Window );
    }

    return Failed;
}


/* Read the schematic header. */
bool ReadSchemaDescr( wxWindow* frame, LINE_READER* aLine, wxString& aMsgDiag, BASE_SCREEN* Window )
{
    char Text[256], buf[1024];
    int  ii;
    Ki_PageDescr*        wsheet = &g_Sheet_A4;
    static Ki_PageDescr* SheetFormatList[] =
    {
        &g_Sheet_A4,   &g_Sheet_A3,   &g_Sheet_A2,   &g_Sheet_A1, &g_Sheet_A0,
        &g_Sheet_A,    &g_Sheet_B,    &g_Sheet_C,    &g_Sheet_D,  &g_Sheet_E,
        &g_Sheet_user, NULL
    };
    wxSize               PageSize;

    sscanf( line, "%s %s %d %d", Text, Text, &PageSize.x, &PageSize.y );

    wxString pagename = CONV_FROM_UTF8( Text );
    for( ii = 0; SheetFormatList[ii] != NULL; ii++ )
    {
        wsheet = SheetFormatList[ii];
        if( wsheet->m_Name.CmpNoCase( pagename ) == 0 ) /* Descr found ! */
        {
            // Get the user page size and make it the default
            if( wsheet == &g_Sheet_user )
            {
                g_Sheet_user.m_Size = PageSize;
            }
            break;
        }
    }

    if( SheetFormatList[ii] == NULL )
    {
        aMsgDiag.Printf( wxT( "EESchema file dimension definition error \
line %d, \aAbort reading file.\n" ),
                         aLine->LineNumber() );
        aMsgDiag << CONV_FROM_UTF8( line );
        DisplayError( frame, aMsgDiag );
    }

    Window->m_CurrentSheetDesc = wsheet;

    for( ; ; )
    {
        if( !aLine->ReadLine() )
            return TRUE;

        if( strnicmp( line, "$End", 4 ) == 0 )
            break;

        if( strnicmp( line, "Sheet", 2 ) == 0 )
            sscanf( line + 5, " %d %d",
                    &Window->m_ScreenNumber, &Window->m_NumberOfScreen );

        if( strnicmp( line, "Title", 2 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Title = CONV_FROM_UTF8( buf );
            continue;
        }

        if( strnicmp( line, "Date", 2 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Date = CONV_FROM_UTF8( buf );
            continue;
        }

        if( strnicmp( line, "Rev", 2 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Revision = CONV_FROM_UTF8( buf );
            continue;
        }

        if( strnicmp( line, "Comp", 4 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Company = CONV_FROM_UTF8( buf );
            continue;
        }

        if( strnicmp( line, "Comment1", 8 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Commentaire1 = CONV_FROM_UTF8( buf );
            continue;
        }

        if( strnicmp( line, "Comment2", 8 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Commentaire2 = CONV_FROM_UTF8( buf );
            continue;
        }

        if( strnicmp( line, "Comment3", 8 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Commentaire3 = CONV_FROM_UTF8( buf );
            continue;
        }

        if( strnicmp( line, "Comment4", 8 ) == 0 )
        {
            ReadDelimitedText( buf, line, 256 );
            Window->m_Commentaire4 = CONV_FROM_UTF8( buf );
            continue;
        }
    }

    return false;
}


/* Function used by LoadEEFile ().
 * Get the lines for a description of a schematic component.
 */

int ReadPartDescr( wxWindow* frame, LINE_READER* aLine, wxString& aMsgDiag, BASE_SCREEN* Window )
{
    int            ii;
    char           Name1[256], Name2[256],
                   Char1[256], Char2[256], Char3[256];
    SCH_COMPONENT* component;
    int            Failed = 0, newfmt = 0;
    char*          ptcar;
    wxString       fieldName;

    component = new SCH_COMPONENT();

    component->m_Convert = 1;

    if( line[0] == '$' )
    {
        newfmt = 1;

        if( !aLine->ReadLine() )
            return TRUE;
    }

    if( sscanf( &line[1], "%s %s", Name1, Name2 ) != 2 )
    {
        aMsgDiag.Printf(
            wxT( "EESchema Component descr error at line %d, aborted" ),
            aLine->LineNumber() );
        aMsgDiag << wxT( "\n" ) << CONV_FROM_UTF8( line );
        Failed = TRUE;
        return Failed;
    }

    if( strcmp( Name1, NULL_STRING ) != 0 )
    {
        for( ii = 0; ii < (int) strlen( Name1 ); ii++ )
            if( Name1[ii] == '~' )
                Name1[ii] = ' ';

        component->m_ChipName = CONV_FROM_UTF8( Name1 );
        if( !newfmt )
            component->GetField( VALUE )->m_Text = CONV_FROM_UTF8( Name1 );
    }
    else
    {
        component->m_ChipName.Empty();
        component->GetField( VALUE )->m_Text.Empty();
        component->GetField( VALUE )->m_Orient    = TEXT_ORIENT_HORIZ;
        component->GetField( VALUE )->m_Attributs = TEXT_NO_VISIBLE;
    }

    if( strcmp( Name2, NULL_STRING ) != 0 )
    {
        bool isDigit = false;
        for( ii = 0; ii < (int) strlen( Name2 ); ii++ )
        {
            if( Name2[ii] == '~' )
                Name2[ii] = ' ';

            // get RefBase from this, too. store in Name1.
            if( Name2[ii] >= '0' && Name2[ii] <= '9' )
            {
                isDigit   = true;
                Name1[ii] = 0;  //null-terminate.
            }
            if( !isDigit )
            {
                Name1[ii] = Name2[ii];
            }
        }

        Name1[ii] = 0; //just in case
        int  jj;
        for( jj = 0; jj<ii && Name1[jj] == ' '; jj++ )
            ;

        if( jj == ii )
        {
            // blank string.
            component->m_PrefixString = wxT( "U" );
        }
        else
        {
            component->m_PrefixString = CONV_FROM_UTF8( &Name1[jj] );

            //printf("prefix: %s\n", CONV_TO_UTF8(component->m_PrefixString));
        }
        if( !newfmt )
            component->GetField( REFERENCE )->m_Text = CONV_FROM_UTF8( Name2 );
    }
    else
    {
        component->GetField( REFERENCE )->m_Attributs = TEXT_NO_VISIBLE;
    }

    /* Parse component description
     * These lines begin with:
     * "P" = Position
     * U = Num Unit and Conversion
     * "Fn" = Fields (0 .. n = = number of field)
     * "Ar" = Alternate reference in the case of multiple sheets referring to
     *        one schematic file.
     */
    for( ; ; )
    {
        if( !aLine->ReadLine() )
            return TRUE;

        if( line[0] == 'U' )
        {
            sscanf( line + 1, "%d %d %lX",
                    &component->m_Multi, &component->m_Convert,
                    &component->m_TimeStamp );
        }
        else if( line[0] == 'P' )
        {
            sscanf( line + 1, "%d %d",
                    &component->m_Pos.x, &component->m_Pos.y );

            // Set fields position to a default position (that is the
            // component position.  For existing fields, the real position
            // will be set later
            for( int i = 0; i<component->GetFieldCount();  ++i )
            {
                if( component->GetField( i )->m_Text.IsEmpty() )
                    component->GetField( i )->m_Pos = component->m_Pos;
            }
        }
        else if( line[0] == 'A' && line[1] == 'R' )
        {
            /* format:
             * AR Path="/9086AF6E/67452AA0" Ref="C99" Part="1"
             * where 9086AF6E is the unique timestamp of the containing sheet
             * and 67452AA0 is the timestamp of this component.
             * C99 is the reference given this path.
             */
            int ii;
            ptcar = line + 2;

            //copy the path.
            ii     = ReadDelimitedText( Name1, ptcar, 255 );
            ptcar += ii + 1;
            wxString path = CONV_FROM_UTF8( Name1 );

            // copy the reference
            ii     = ReadDelimitedText( Name1, ptcar, 255 );
            ptcar += ii + 1;
            wxString ref = CONV_FROM_UTF8( Name1 );

            // copy the multi, if exists
            ii = ReadDelimitedText( Name1, ptcar, 255 );
            if( Name1[0] == 0 )  // Nothing read, put a default value
                sprintf( Name1, "%d", component->m_Multi );
            int multi = atoi( Name1 );
            if( multi < 0 || multi > 25 )
                multi = 1;
            component->AddHierarchicalReference( path, ref, multi );
            component->GetField( REFERENCE )->m_Text = ref;
        }
        else if( line[0] == 'F' )
        {
            int  fieldNdx;

            char FieldUserName[1024];
            GRTextHorizJustifyType hjustify = GR_TEXT_HJUSTIFY_CENTER;
            GRTextVertJustifyType  vjustify = GR_TEXT_VJUSTIFY_CENTER;

            FieldUserName[0] = 0;

            ptcar = line;

            while( *ptcar && (*ptcar != '"') )
                ptcar++;

            if( *ptcar != '"' )
            {
                aMsgDiag.Printf(
                    wxT( "EESchema file lib field F at line %d, aborted" ),
                    aLine->LineNumber() );
                return TRUE;
            }

            for( ptcar++, ii = 0; ; ii++, ptcar++ )
            {
                Name1[ii] = *ptcar;
                if( *ptcar == 0 )
                {
                    aMsgDiag.Printf(
                        wxT( "Component field F at line %d, aborted" ),
                        aLine->LineNumber() );
                    return TRUE;
                }

                if( *ptcar == '"' )
                {
                    Name1[ii] = 0;
                    ptcar++;
                    break;
                }
            }

            fieldNdx = atoi( line + 2 );

            ReadDelimitedText( FieldUserName, ptcar, sizeof(FieldUserName) );

            if( !FieldUserName[0] )
                fieldName = TEMPLATE_FIELDNAME::GetDefaultFieldName( fieldNdx );
            else
                fieldName = CONV_FROM_UTF8( FieldUserName );

            if( fieldNdx >= component->GetFieldCount() )
            {
                // The first MANDATOR_FIELDS _must_ be constructed within
                // the SCH_COMPONENT constructor.  This assert is simply here
                // to guard against a change in that constructor.
                wxASSERT( component->GetFieldCount() >= MANDATORY_FIELDS );

                // Ignore the _supplied_ fieldNdx.  It is not important anymore
                // if within the user defined fields region (i.e. >= MANDATORY_FIELDS).
                // We freely renumber the index to fit the next available field slot.

                fieldNdx = component->GetFieldCount();  // new has this index after insertion

                SCH_FIELD field( wxPoint( 0, 0 ),
                            -1,     // field id is not relavant for user defined fields
                            component, fieldName );

                component->AddField( field );
            }
            else
            {
                component->GetField( fieldNdx )->m_Name = fieldName;
            }

            component->GetField( fieldNdx )->m_Text = CONV_FROM_UTF8( Name1 );
            memset( Char3, 0, sizeof(Char3) );
            if( ( ii = sscanf( ptcar, "%s %d %d %d %X %s %s", Char1,
                               &component->GetField( fieldNdx )->m_Pos.x,
                               &component->GetField( fieldNdx )->m_Pos.y,
                               &component->GetField( fieldNdx )->m_Size.x,
                               &component->GetField( fieldNdx )->m_Attributs,
                               Char2, Char3 ) ) < 4 )
            {
                aMsgDiag.Printf(
                    wxT( "Component Field error line %d, aborted" ),
                    aLine->LineNumber() );
                DisplayError( frame, aMsgDiag );
                continue;
            }

            if( (component->GetField( fieldNdx )->m_Size.x == 0 ) || (ii == 4) )
                component->GetField( fieldNdx )->m_Size.x = DEFAULT_SIZE_TEXT;

            component->GetField( fieldNdx )->m_Orient = TEXT_ORIENT_HORIZ;
            component->GetField( fieldNdx )->m_Size.y =
                component->GetField( fieldNdx )->m_Size.x;

            if( Char1[0] == 'V' )
                component->GetField( fieldNdx )->m_Orient = TEXT_ORIENT_VERT;

            if( ii >= 7 )
            {
                if( *Char2 == 'L' )
                    hjustify = GR_TEXT_HJUSTIFY_LEFT;
                else if( *Char2 == 'R' )
                    hjustify = GR_TEXT_HJUSTIFY_RIGHT;
                if( Char3[0] == 'B' )
                    vjustify = GR_TEXT_VJUSTIFY_BOTTOM;
                else if( Char3[0] == 'T' )
                    vjustify = GR_TEXT_VJUSTIFY_TOP;
                if( Char3[1] == 'I' )
                    component->GetField( fieldNdx )->m_Italic = true;
                else
                    component->GetField( fieldNdx )->m_Italic = false;
                if( Char3[2] == 'B' )
                    component->GetField( fieldNdx )->m_Bold = true;
                else
                    component->GetField( fieldNdx )->m_Bold = false;

                component->GetField( fieldNdx )->m_HJustify = hjustify;
                component->GetField( fieldNdx )->m_VJustify = vjustify;
            }

            if( fieldNdx == REFERENCE )
                if( component->GetField( fieldNdx )->m_Text[0] == '#' )
                    component->GetField( fieldNdx )->m_Attributs |=
                        TEXT_NO_VISIBLE;
        }
        else
            break;
    }

    if( sscanf( line, "%d %d %d", &component->m_Multi,
                &component->m_Pos.x, &component->m_Pos.y ) != 3 )
    {
        aMsgDiag.Printf(
            wxT( "Component unit & pos error at line %d, aborted" ),
            aLine->LineNumber() );
        Failed = TRUE;
        return Failed;
    }

    if( !aLine->ReadLine() ||
        sscanf( line, "%d %d %d %d",
                    &component->m_Transform.x1,
                    &component->m_Transform.y1,
                    &component->m_Transform.x2,
                    &component->m_Transform.y2 ) != 4 )
    {
        aMsgDiag.Printf( wxT( "Component orient error at line %d, aborted" ), aLine->LineNumber() );
        Failed = TRUE;
        return Failed;
    }

    if( newfmt )
    {
        if( !aLine->ReadLine() )
            return TRUE;

        if( strnicmp( "$End", line, 4 ) != 0 )
        {
            aMsgDiag.Printf(
                wxT( "Component End expected at line %d, aborted" ),
                aLine->LineNumber() );
            Failed = TRUE;
        }
    }

    if( !Failed )
    {
        component->SetNext( Window->EEDrawList );
        Window->EEDrawList = component;
        component->SetParent( Window );
    }

    return Failed;
}
