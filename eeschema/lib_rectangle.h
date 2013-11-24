/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2004-2011 KiCad Developers, see change_log.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file lib_rectangle.h
 */

#ifndef _LIB_RECTANGLE_H_
#define _LIB_RECTANGLE_H_

#include <lib_draw_item.h>


class LIB_RECTANGLE  : public LIB_ITEM
{
    wxPoint m_End;                  // Rectangle end point.
    wxPoint m_Pos;                  // Rectangle start point.
    int     m_Width;                // Line width
    bool    m_isWidthLocked;        // Flag: Keep width locked
    bool    m_isHeightLocked;       // Flag: Keep height locked
    bool    m_isStartPointSelected; // Flag: is the upper left edge selected?

    void drawGraphic( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aOffset,
                      EDA_COLOR_T aColor, GR_DRAWMODE aDrawMode, void* aData,
                      const TRANSFORM& aTransform );

    void calcEdit( const wxPoint& aPosition );

public:
    LIB_RECTANGLE( LIB_COMPONENT * aParent );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~LIB_RECTANGLE() { }

    wxString GetClass() const
    {
        return wxT( "LIB_RECTANGLE" );
    }

    void SetEndPosition( const wxPoint& aPosition ) { m_End = aPosition; }

    bool Save( OUTPUTFORMATTER& aFormatter );

    bool Load( LINE_READER& aLineReader, wxString& aErrorMsg );

    bool HitTest( const wxPoint& aPosition );

    bool HitTest( wxPoint aPosRef, int aThreshold, const TRANSFORM& aTransform );

    int GetPenSize( ) const;

    const EDA_RECT GetBoundingBox() const;    // Virtual

    void GetMsgPanelInfo( std::vector< MSG_PANEL_ITEM >& aList );

    void BeginEdit( STATUS_FLAGS aEditMode, const wxPoint aStartPoint = wxPoint( 0, 0 ) );

    bool ContinueEdit( const wxPoint aNextPoint );

    void EndEdit( const wxPoint& aPosition, bool aAbort = false );

    void SetOffset( const wxPoint& aOffset );

    bool Inside( EDA_RECT& aRect ) const;

    void Move( const wxPoint& aPosition );

    wxPoint GetPosition() const { return m_Pos; }

    void MirrorHorizontal( const wxPoint& aCenter );

    void MirrorVertical( const wxPoint& aCenter );

    void Rotate( const wxPoint& aCenter, bool aRotateCCW = true );

    void Plot( PLOTTER* aPlotter, const wxPoint& aOffset, bool aFill,
               const TRANSFORM& aTransform );

    int GetWidth() const { return m_Width; }

    void SetWidth( int aWidth ) { m_Width = aWidth; }

    wxString GetSelectMenuText() const;

    BITMAP_DEF GetMenuImage() const { return  add_rectangle_xpm; }

    EDA_ITEM* Clone() const;

private:

    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The rectangle specific sort order is as follows:
     *      - Rectangle horizontal (X) start position.
     *      - Rectangle vertical (Y) start position.
     *      - Rectangle horizontal (X) end position.
     *      - Rectangle vertical (Y) end position.
     */
    int compare( const LIB_ITEM& aOther ) const;
};


#endif    // _LIB_RECTANGLE_H_
