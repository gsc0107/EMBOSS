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
*
*  @author: Copyright (C) Tim Carver
*
********************************************************************/

package org.emboss.jemboss.gui.filetree;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

import org.emboss.jemboss.soap.JembossSoapException;
import org.emboss.jemboss.JembossParams;

public class LocalAndRemoteFileTreeFrame extends JFrame
{

  private static DragTree ltree;
  private JSplitPane treePane;
  private JMenuItem prefClose;

/**
*
* @param mysettings JembossParams with settings information
*/
  public LocalAndRemoteFileTreeFrame(final JembossParams mysettings) 
                                throws JembossSoapException
  {
    super("File Manager");
    try
    {  
      final RemoteFileTreePanel rtree =
                           new RemoteFileTreePanel(mysettings,false);
      ltree = new DragTree(new File(System.getProperty("user.home")), 
                                                   this, mysettings);
      final JScrollPane scrollTree = new JScrollPane(ltree);   

      Dimension d = rtree.getPreferredSize();
      scrollTree.setPreferredSize(d);
      JPanel jp = new JPanel(new BorderLayout());

      JMenuBar menuBar = new JMenuBar();
      JMenu prefMenu = new JMenu("File");
      prefMenu.setMnemonic(KeyEvent.VK_F);
      final JFrame f = this;

      ButtonGroup group = new ButtonGroup();
      JRadioButtonMenuItem prefV = new JRadioButtonMenuItem("Vertical Split");
      prefMenu.add(prefV);
      prefV.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          treePane.remove(rtree);
          treePane.remove(scrollTree);
          treePane.setOrientation(JSplitPane.VERTICAL_SPLIT);
          treePane.setTopComponent(rtree);
          treePane.setBottomComponent(scrollTree);
          treePane.setDividerLocation(0.5);
          Dimension pSize = new Dimension(210, 250);
          rtree.setPreferredSize(pSize);
          scrollTree.setPreferredSize(pSize);
          f.pack();
          treePane.setDividerLocation(0.5);
        }
      });
      prefV.setSelected(true);
      group.add(prefV);
     
      JRadioButtonMenuItem prefH = new JRadioButtonMenuItem("Horizontal Split");
      prefMenu.add(prefH);
      prefH.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          treePane.remove(rtree);
          treePane.remove(scrollTree);
          treePane.setOrientation(JSplitPane.HORIZONTAL_SPLIT);
          treePane.setRightComponent(rtree);
          treePane.setLeftComponent(scrollTree);
          Dimension pSize = new Dimension(210, 250);
          rtree.setPreferredSize(pSize);
          scrollTree.setPreferredSize(pSize);
          f.pack();
          treePane.setDividerLocation(0.5);
        }
      });
      group.add(prefH);

      prefClose = new JMenuItem("Close");
      prefClose.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_C, ActionEvent.CTRL_MASK));

      prefClose.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          f.setVisible(false);
        }
      });
      prefMenu.addSeparator();
      prefMenu.add(prefClose);
      menuBar.add(prefMenu);

      JComboBox rootSelect = rtree.getRootSelect();
      rootSelect.setMaximumSize(d);

      menuBar.add(Box.createHorizontalGlue());
      menuBar.add(rootSelect);
      setJMenuBar(menuBar);
      
      Dimension pSize = new Dimension(210, 250);
      rtree.setPreferredSize(pSize);
      scrollTree.setPreferredSize(pSize);
      treePane = new JSplitPane(JSplitPane.VERTICAL_SPLIT,
                                        rtree,scrollTree);
      treePane.setOneTouchExpandable(true);
      getContentPane().add(treePane);
      pack();
      setVisible(true);
    }
    catch(JembossSoapException jse)
    {
      throw new JembossSoapException();
    }
  }

  public void setExit()
  {
    prefClose.setText("Exit");
    prefClose.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        System.exit(0);
      }
    });

  }

  public static DragTree getLocalDragTree()
  {
    return ltree;
  }

}

