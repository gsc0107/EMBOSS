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

package org.emboss.jemboss.gui;

import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;

import java.awt.event.*;
import java.io.*;
import java.util.*;


import org.emboss.jemboss.programs.*;        // running EMBOSS programs
import org.emboss.jemboss.gui.startup.*;     // finds programs, groups, docs & db's
import org.emboss.jemboss.soap.*;
import org.emboss.jemboss.gui.form.*;        // program forms constructed from ACD
import org.emboss.jemboss.soap.GetWossname;
import uk.ac.mrc.hgmp.embreo.*;

/**
*
* BuildProgramMenu class construct the program menus.
*
* @author T. J. Carver
* @version 1.0 
*
*/

public class BuildProgramMenu
{

  private String db[];                           // database names
  private String applName;
  private Hashtable acdStore = new Hashtable();  // cache the acd files
  private JFrame f;
  private JPanel p1;
  private JPanel p2;
  private JScrollPane scrollProgForm;

  private String embossBin;
  private String envp[];
  private EmbreoParams mysettings;
  private boolean withSoap;
  private String cwd;
  private String acdDirToParse;
  private String showdbOut;

  final Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  final Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);

/**
*
*  @param  JPanel p1 is the menu pane
*  @param  JPanel p2 is the form pane
*  @param  JScrollPane EMBOSS form scroll pane 
*  @param  String location of the EMBOSS binaries
*  @param  String array of environment variables for EMBOSS applications.
*  @param  EmbreoParams SOAP parameter settings
*  @param  boolean true if using SOAP server
*  @param  String current working directory (local)
*  @param  String location of the ACD directory
*  @param  JFrame Jemboss frame
*  @param  AuthPopup splash frame
*
*/
  public BuildProgramMenu(final JPanel p1, final JPanel p2, 
           final JScrollPane scrollProgForm, final String embossBin,
           final String envp[], final EmbreoParams mysettings, 
           final boolean withSoap, final String cwd,
           final String acdDirToParse, JFrame frame, final AuthPopup splashing)
  {
  
    f = frame;
    this.p1 = p1;
    this.p2 = p2;
    this.scrollProgForm = scrollProgForm;
    this.embossBin = embossBin;
    this.envp = envp;
    this.mysettings = mysettings;
    this.withSoap = withSoap;
    this.cwd = cwd;
    this.acdDirToParse = acdDirToParse;

    SwingWorker groupworker = new SwingWorker() 
    {

      String woss = "";

      public Object construct() 
      {
        if(withSoap) 
        {
          splashing.doneSomething("Connecting with server");
          GetWossname ewoss = new GetWossname(mysettings);
          woss = ewoss.getDBText();
          splashing.doneSomething("Found EMBOSS applications");
        } 
        else 
        {
    
          String embossCommand = new String(embossBin + "wossname -colon -auto");
          System.out.println(embossCommand);
          RunEmbossApplication rea = new RunEmbossApplication(embossCommand,envp,null);
          rea.isProcessStdout();
          woss = rea.getProcessStdout();
          Process processWoss = rea.getProcess();

          embossCommand = new String(embossBin + "showdb -auto");
          rea = new RunEmbossApplication(embossCommand,envp,null);
          rea.isProcessStdout();
          showdbOut = rea.getProcessStdout();

          try 
          {
            processWoss.waitFor();
          } 
          catch (InterruptedException interre)
          {
            System.out.println("BuildProgramMenu received interruption error");
          }
          Database d = new Database(showdbOut);
          db = d.getDB();
          
        }

        return woss;
      }


      public void finished() 
      {

// program menu
        JMenuBar menuBar = new JMenuBar();
        ProgList progs = new ProgList(woss,cwd,menuBar);

        if(withSoap)
          splashing.doneSomething("Constructing menus");

        int npG = progs.getNumPrimaryGroups();
        menuBar.setLayout(new  GridLayout(npG,1));
   
        final int numProgs = progs.getNumProgs();
        JMenu primaryGroups[] = new JMenu[npG];
        primaryGroups = progs.getPrimaryGroups();

        final String allAcd[] = progs.getProgsList();
        final String allDes[] = progs.getProgDescription();
//      Component c = p1.getComponent(0);
//      p1.remove(c);

        p1.add(menuBar, BorderLayout.NORTH);
        f.setVisible(true);

        JMenuItem mi[] = new JMenuItem[numProgs];
        mi = progs.getMenuItems();
        int nm = progs.getNumberMenuItems();

// create action listeners into menu to build Jemboss program forms
        for(int i=0; i<nm;i++)
        {
          mi[i].addActionListener(new ActionListener()
          {
            public void actionPerformed(ActionEvent e)
            {
              f.setCursor(cbusy);
              JMenuItem source = (JMenuItem)(e.getSource());
              for(int k=0;k<numProgs;k++)
              {
                String p = source.getText();
                int ind = p.indexOf(" ");
                p = p.substring(0,ind).trim();
                if(p.equalsIgnoreCase(allAcd[k]))
                {
                  applName = allAcd[k];
                  p2.removeAll();
                  BuildJembossForm bjf = new BuildJembossForm(k,allDes,db,
                      allAcd,envp,cwd,embossBin,acdDirToParse,withSoap,p2,
                      mysettings,acdStore,f);
                  
                  p2.setVisible(false);
                  p2.setVisible(true);
                  JViewport vp = scrollProgForm.getViewport();
                  vp.setViewPosition(new Point(0,0));
                  break;
                }
              }
              f.setCursor(cdone);
            }
          });
//        f.setVisible(true);

          if(withSoap)
          {
            JFrame splashf = splashing.getSplashFrame();
            if(splashf.isVisible())
              splashf.toFront();
          }
        }

// program scroll list
        final JList progList = new JList(allAcd);
        JScrollPane scrollPane = new JScrollPane(progList);

        Box alphaPane = new Box(BoxLayout.Y_AXIS);
        Box alphaTextPane = new Box(BoxLayout.X_AXIS);
        alphaPane.add(Box.createRigidArea(new Dimension(0,10)));
        alphaTextPane.add(new JLabel("GoTo:"));
        alphaTextPane.add(Box.createRigidArea(new Dimension(5,0)));

        final JTextField alphaTextPaneEntry = new JTextField(12);
        alphaTextPaneEntry.setMaximumSize(new Dimension(100,20));
        alphaTextPaneEntry.getDocument().addDocumentListener(new DocumentListener()
        {
          public void insertUpdate(DocumentEvent e)
          {
            updateScroll();
          }
          public void removeUpdate(DocumentEvent e) 
          {
            updateScroll();
          }
          public void changedUpdate(DocumentEvent e) {}
          public void updateScroll()
          {
            for(int k=0;k<numProgs;k++)
              if(allAcd[k].startsWith(alphaTextPaneEntry.getText()))
              {
                progList.ensureIndexIsVisible(k);
                progList.setSelectionBackground(Color.cyan);
                progList.setSelectedIndex(k);
                break;
              }
          }
        });
        alphaTextPane.add(alphaTextPaneEntry);
        alphaPane.add(alphaTextPane);
        alphaPane.add(scrollPane);

        p1.add(alphaPane, BorderLayout.CENTER);

        Dimension dp1 = p1.getMinimumSize();
        dp1 = new Dimension((int)dp1.getWidth()-10,(int)dp1.getHeight());
        p1.setPreferredSize(dp1);
        p1.setMaximumSize(dp1);
        p1.setMinimumSize(dp1);

        progList.setSelectionBackground(Color.cyan);

// create listener to build Jemboss program forms
        MouseListener mouseListener = new MouseAdapter()
        {
          public void mouseClicked(MouseEvent e)
          {
            f.setCursor(cbusy);
            JList source = (JList)e.getSource();
            source.setSelectionBackground(Color.cyan);
            int index = source.getSelectedIndex();
            applName = allAcd[index];
            p2.removeAll();
            BuildJembossForm bjf = new BuildJembossForm(index,allDes,db,
                    allAcd,envp,cwd,embossBin,acdDirToParse,withSoap,p2,
                    mysettings,acdStore,f);
            p2.setVisible(false);
            p2.setVisible(true);
            JViewport vp = scrollProgForm.getViewport();
            vp.setViewPosition(new Point(0,0));
            f.setCursor(cdone);
          }
        };
        progList.addMouseListener(mouseListener);

        p1.setVisible(false);
        p1.setVisible(true);


      }
    };
    groupworker.start();

    if(withSoap)
    {
      SwingWorker databaseworker = new SwingWorker()
      {
        public Object construct()
        {
          EmbreoShowDB showdb = new EmbreoShowDB(mysettings);
          showdbOut = showdb.getDBText();
          Database d = new Database(showdbOut);
          db = d.getDB();
          return null;
        }
      };
      databaseworker.start();
    }

  }

}


