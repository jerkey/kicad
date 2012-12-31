/**
 * @file tree_project_frame.h
 */

/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 1992-2012 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 1992-2012 KiCad Developers, see change_log.txt for contributors.
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

#ifndef TREEPRJ_FRAME_H
#define TREEPRJ_FRAME_H

#if wxCHECK_VERSION( 2, 9, 2  )
#include <wx/fswatcher.h>
#endif

class TREEPROJECT_ITEM;

/** class TREE_PROJECT_FRAME
 * Window to display the tree files
 */
class TREE_PROJECT_FRAME : public wxSashLayoutWindow
{
    friend class TREEPROJECT_ITEM;
public:
    KICAD_MANAGER_FRAME*    m_Parent;
    TREEPROJECTFILES*       m_TreeProject;
    wxTreeItemId            m_root;
private:
    std::vector<wxMenu*>    m_ContextMenus;
    std::vector<wxString>   m_filters;
    wxMenu*                 m_PopupMenu;
    wxCursor                m_DragCursor;
    wxCursor                m_Default;

#if wxCHECK_VERSION( 2, 9, 2  )
    wxFileSystemWatcher*    m_watcher; // file system watcher
#endif

public:
    TREE_PROJECT_FRAME( KICAD_MANAGER_FRAME* parent );
    ~TREE_PROJECT_FRAME();
    void                            ReCreateTreePrj();
#if wxCHECK_VERSION( 2, 9, 2  )
    void                            FileWatcherReset();
#endif

protected:
    wxMenu*                         GetContextMenu( int type );
    static wxString                 GetFileExt( TreeFileType type );
    static wxString                 GetFileWildcard( TreeFileType type );

    /**
     * Function GetSelectedData
     * return the item data from item currently selected (highlighted)
     * Note this is not necessary the "clicked" item,
     * because when expanding, collapsing an item this item is not selected
     */
    TREEPROJECT_ITEM*               GetSelectedData();

    /**
     * Function GetItemIdData
     * return the item data corresponding to a wxTreeItemId identifier
     * @param  aId = the wxTreeItemId identifier.
     * @return a TREEPROJECT_ITEM pointer correspondinfg to item id aId
     */
    TREEPROJECT_ITEM*               GetItemIdData( wxTreeItemId aId );

private:
    void                            OnSelect( wxTreeEvent& Event );
    void                            OnExpand( wxTreeEvent& Event );
    void                            OnDragStart( wxTreeEvent& event );
    void                            OnDragEnd( wxTreeEvent& event );
    void                            OnRight( wxTreeEvent& Event );

    /**
     * Function OnOpenSelectedFileWithTextEditor
     * Called via the popup menu, when right clicking on a file name
     * Call the text editor to open the selected file
     * in the tree project
     */
    void                            OnOpenSelectedFileWithTextEditor( wxCommandEvent& event );

    /**
     * Function OnDeleteFile
     * Called via the popup menu, when right clicking on a file name
     * or a directory name to delete the selected file or directory
     * in the tree project
     */
    void                            OnDeleteFile( wxCommandEvent& event );

    /**
     * Function OnRenameFile
     * Called via the popup menu, when right clicking on a file name
     * or a directory name to rename the selected file or directory
     * in the tree project
     */
    void                            OnRenameFile( wxCommandEvent& event );

    /**
     * Function OnCreateNewDirectory
     * Creates a new subdirectory inside the current kicad project directory
     * the user is prompted to enter a directory name
     */
    void                            OnCreateNewDirectory( wxCommandEvent& event );

    void                            ClearFilters()
    {
        m_filters.clear();
    }

    const std::vector<wxString>&    GetFilters()
    {
        return m_filters;
    }

    void                            RemoveFilter( const wxString& filter );

    /**
     * Function AddItemToTreeProject
     * @brief  Add the file or directory aName to the project tree
     * @param aName = the filename or the directory name to add in tree
     * @param aRoot = the wxTreeItemId item where to add sub tree items
     * @param aRecurse = true to add file or subdir names to the current tree item
     *                   false to stop file add.
     * @return true if the file (or directory) is added.
     */
    bool                            AddItemToTreeProject( const wxString& aName,
                                                          wxTreeItemId& aRoot,
                                                          bool aRecurse = true );

    /**
     * function findSubdirTreeItem
     * Search for the item in tree project which is the
     * node of the subdirectory aSubDir
     * @param aSubDir = the directory to find in tree
     * @return the opaque reference to the tree item.
     * if not found, return an invalid tree item.
     * therefore wxTreeItemId::IsOk should be used to test
     * the returned value
     */
    wxTreeItemId findSubdirTreeItem( const wxString& aSubDir );

#if wxCHECK_VERSION( 2, 9, 2  )
    void                            OnFileSystemEvent( wxFileSystemWatcherEvent& event );
#endif

    DECLARE_EVENT_TABLE()
};

#endif    // TREEPRJ_FRAME_H
