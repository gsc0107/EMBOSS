/********************************************************************
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Library General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Library General Public License for more details.
*
*  You should have received a copy of the GNU Library General Public
*  License along with this library; if not, write to the
*  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
*  Boston, MA  02111-1307, USA.
*
*  @author: Copyright (C) Tim Carver
*
********************************************************************/

package org.emboss.jemboss.gui.filetree;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.awt.dnd.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import java.io.*;
import java.util.*;

import org.emboss.jemboss.gui.ResultsMenuBar;

/**
*
* Creates a local file tree for Jemboss. This acts as a drag 
* source and sink for files.
*
*/
public class DragTree extends JTree implements DragGestureListener,
                           DragSourceListener, DropTargetListener 
{

  public static DefaultTreeModel model;
  private String fs = new String(System.getProperty("file.separator"));
  private Hashtable openNodeDir;

  final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);


  public DragTree(final File root, final JFrame f) 
  {
    openNodeDir = new Hashtable();
    DragSource dragSource = DragSource.getDefaultDragSource();

    setDropTarget(new DropTarget(this,this));
    dragSource.createDefaultDragGestureRecognizer(
               this,                             // component where drag originates
               DnDConstants.ACTION_COPY_OR_MOVE, // actions
               this);                            // drag gesture recognizer

    model = createTreeModel(root);
    setModel(model);
    createTreeModelListener();


    this.getSelectionModel().setSelectionMode
                  (TreeSelectionModel.SINGLE_TREE_SELECTION);

    //Listen for when a file is selected

    MouseListener mouseListener = new MouseAdapter() {
      public void mouseClicked(MouseEvent me) 
      {
        if(me.getClickCount() == 2 && isFileSelection()) 
        {
          f.setCursor(cbusy);
          JTree t = (JTree)me.getSource();
          TreePath p = t.getSelectionPath();
          DefaultMutableTreeNode node = (DefaultMutableTreeNode)
                                       t.getLastSelectedPathComponent();
          String selected = node.toString();

          TreeNode tn = node.getParent();
          selected = tn.toString().concat(fs + selected);
          while((tn = tn.getParent()) != null) 
            selected = tn.toString().concat(fs + selected);
          int sep = selected.indexOf(fs);
          selected = selected.substring(sep+1,selected.length());
          selected = root.toString().concat(fs +selected);
          showFilePane(selected);
          f.setCursor(cdone);
        }
      }
    };

    this.addMouseListener(mouseListener);

    addTreeExpansionListener(new TreeExpansionListener(){
      public void treeCollapsed(TreeExpansionEvent e) 
      {
      }
      public void treeExpanded(TreeExpansionEvent e) 
      {
        TreePath path = e.getPath();
        if(path != null) 
        {
          FileNode node = (FileNode)path.getLastPathComponent();

          if(!node.isExplored()) 
          {  
            model = (DefaultTreeModel)getModel();
            node.explore(openNodeDir);
            model.nodeStructureChanged(node);
          }
        }
      }
    });


  }

// drag source
  public void dragGestureRecognized(DragGestureEvent e) 
  {
    // drag only files 
    if(isFileSelection())
      e.startDrag(DragSource.DefaultCopyDrop, // cursor
                 (Transferable)getNodename(), // transferable data
                                       this); // drag source listener
  }
  public void dragDropEnd(DragSourceDropEvent e) {}
  public void dragEnter(DragSourceDragEvent e) {}
  public void dragExit(DragSourceEvent e) {}
  public void dragOver(DragSourceDragEvent e) {}
  public void dropActionChanged(DragSourceDragEvent e) {}

// drop sink
  public void dragEnter(DropTargetDragEvent e)
  {
    if(e.isDataFlavorSupported(RemoteFileNode.REMOTEFILENODE))
    {
      e.acceptDrag(DnDConstants.ACTION_COPY_OR_MOVE);
      System.out.println("dragEnter");
    }
  }

  public void dragExit (DropTargetEvent event) {}

/**
*
* When a suitable DataFlavor is offered over a remote file
* node the node is highlighted/selected and the drag
* accepted. Otherwise the drag is rejected.
*
*/
  public void dragOver(DropTargetDragEvent e)
  {
    if (e.isDataFlavorSupported(RemoteFileNode.REMOTEFILENODE))
    {
      Point ploc = e.getLocation();
      TreePath ePath = getPathForLocation(ploc.x,ploc.y);
      if (ePath == null)
        e.rejectDrag();
      else
      {
        setSelectionPath(ePath);
        e.acceptDrag(DnDConstants.ACTION_COPY_OR_MOVE);
      }
    }
    else
      e.rejectDrag();
    
  }

  public void dropActionChanged(DropTargetDragEvent event) {}

  public void drop (DropTargetDropEvent event) 
  {
    
  }

  public String getFilename()
  {
    TreePath path = getLeadSelectionPath();
    FileNode node = (FileNode)path.getLastPathComponent();
    return ((File)node.getUserObject()).getAbsolutePath();
  }

  public FileNode getNodename()
  {
    TreePath path = getLeadSelectionPath();
    FileNode node = (FileNode)path.getLastPathComponent();
    return node;
  }

  public boolean isFileSelection() 
  {
    TreePath path = getLeadSelectionPath();
    FileNode node = (FileNode)path.getLastPathComponent();
    return !node.isDirectory();
  }


  private DefaultTreeModel createTreeModel(File root) 
  {
    FileNode rootNode = new FileNode(root,openNodeDir);
    rootNode.explore(openNodeDir);
    return new DefaultTreeModel(rootNode);
  }

  public DefaultTreeModel getTreeModel () 
  {
    return model;
  }

/**
*
* Adding a file (or directory) to the file tree manager.
* This looks to see if the directory has already been opened
* and updates the filetree if it has.
* @param file to add to the tree
*
*/
  public DefaultMutableTreeNode addObject(String child, String path)
  {

    FileNode parentNode = null;
    Enumeration enum = openNodeDir.keys();
    while(enum.hasMoreElements())
    {
      String thiskey = (String)enum.nextElement();
      if(thiskey.equals(path))
      {
//      System.out.println("OPEN NODE EQUALS " + thiskey +  " :::: " + path  +  " :::: " + child);
        parentNode = (FileNode)openNodeDir.get(thiskey);
        break;
      }
    }

    if(parentNode == null)
      return null;
    else if(!parentNode.isExplored()) 
    {
      model = (DefaultTreeModel)getModel();
      parentNode.explore(openNodeDir);
      model.nodeStructureChanged(parentNode);
    }

    File newleaf = new File(child);
    int index = parentNode.getAnIndex(child);
    FileNode childNode = new FileNode(newleaf,openNodeDir);
    model.insertNodeInto(childNode, parentNode, index);

   // Make sure the user can see the new node.
    this.scrollPathToVisible(new TreePath(childNode.getPath()));

    return childNode;
  }


/**
*
* Opens a JFrame with the file contents displayed.
* @param the file name
*
*/
  public void showFilePane(String filename)
  {
    JFrame ffile = new JFrame(filename);
    JPanel pfile = (JPanel)ffile.getContentPane();
    pfile.setLayout(new BorderLayout());
    JPanel pscroll = new JPanel(new BorderLayout());
    JScrollPane rscroll = new JScrollPane(pscroll);

    FileEditorDisplay fed = new FileEditorDisplay(ffile, filename);
    new ResultsMenuBar(ffile,fed);
    pfile.add(rscroll, BorderLayout.CENTER);
    JTextPane seqText = fed.getJTextPane();
    pscroll.add(seqText, BorderLayout.CENTER);
    seqText.setCaretPosition(0);
    ffile.setSize(450,400);
    ffile.setVisible(true);
  }

}

