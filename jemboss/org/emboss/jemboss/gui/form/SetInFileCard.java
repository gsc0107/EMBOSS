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


package org.emboss.jemboss.gui.form;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

import org.emboss.jemboss.gui.SetUpMenuBar;
import org.emboss.jemboss.gui.sequenceChooser.*;


public class SetInFileCard
{

  private Box boxFile;
  private boolean isFile = true;
  private boolean isCut  = false;
  private boolean isList = false;
  private FileChooser fileChoose;
  private CutNPasteTextArea cutnPaste;
  private ListFilePanel listPane = null;
  private InputSequenceAttributes inSeqAttr[];
  private int h;

/**
*
* Build the GUI components for an input sequence(s).
* @param sectionPane acd section panel
* @param h GUI handle
* @param db database list
* @param name for the file card
* @param appName application name
* @param inSeqAttr input sequence attributes
* @param boolean true if list files allowed
*
*/
  public SetInFileCard(final JPanel sectionPane, final int h,
                 final String db[], String name, final String appName,
                 final InputSequenceAttributes inSeqAttr[], boolean fopt)
  {

    this.h = h;
    this.inSeqAttr = inSeqAttr;

    boxFile = new Box(BoxLayout.Y_AXIS);
    final CardLayout fileCard = new CardLayout();
    final JPanel pfile = new JPanel(fileCard);
    Dimension d = new Dimension(500, 130);
    pfile.setPreferredSize(d);
    pfile.setMinimumSize(d);
    pfile.setMaximumSize(d);

    Font labfont = SectionPanel.labfont;
    Color labelColor = SectionPanel.labelColor;

    final Box bdown[] = new Box[3];
    Box bacross = Box.createHorizontalBox();
    JRadioButton rfile  = new JRadioButton("file / database entry");
    rfile.setFont(labfont);
  
    rfile.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        fileCard.show(pfile, "File");
        isFile = true;
        isCut  = false;
        isList = false;
      }
    });
    rfile.setSelected(true);

    JRadioButton rpaste = new JRadioButton ("paste");
    rpaste.setFont(labfont);
    rpaste.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
        fileCard.show(pfile, "Paste");
        isFile = false;
        isCut  = true;
        isList = false;
      }
    });

    JRadioButton rlist = new JRadioButton ("list of files");
    rlist.setFont(labfont);
    rlist.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e)
      {
        if(listPane==null)  // create the list panel here 
        {
          listPane = new ListFilePanel(15);
          Box bxleft = new Box(BoxLayout.X_AXIS);
          bxleft.add(listPane);
          bxleft.add(Box.createHorizontalGlue());
          bdown[2].add(bxleft);
        }
        fileCard.show(pfile, "List");
        isFile = false;
        isCut  = false;
        isList = true;
      }
    });

    ButtonGroup group = new ButtonGroup();
    group.add(rfile);
    group.add(rpaste);
    group.add(rlist);  

    JLabel seqLabel = new JLabel("Enter the sequence as:"); 
    seqLabel.setForeground(labelColor);
    bacross.add(seqLabel);
    bacross.add(Box.createRigidArea(new Dimension(20,0)));
    bacross.add(Box.createHorizontalGlue());
    boxFile.add(bacross);

    bacross = Box.createHorizontalBox();
    bacross.add(rfile);
    bacross.add(new JLabel(" or  "));
    bacross.add(rpaste);

    if(fopt)
    {
      bacross.add(new JLabel(" or  "));
      bacross.add(rlist);
    }

    bacross.add(Box.createHorizontalGlue());
    boxFile.add(bacross);
    boxFile.add(Box.createVerticalStrut(8));

    for(int k=0; k<3; k++)
      bdown[k] =  Box.createVerticalBox();

    fileChoose = new FileChooser(bdown[0],name);

//find any default sequence in the user's SequenceList
    String defaultSeq = SetUpMenuBar.seqList.getDefaultSequenceName();
    if(defaultSeq != null)
      fileChoose.setText(defaultSeq);
   

//sequence attibute options
    final JButton boption = new JButton("Input Sequence Options");
    final JButton breset = new JButton("Reset");
    fileChoose.setSize(boption.getPreferredSize());
    fileChoose.setForeground(labelColor);

    Box bxleft= new Box(BoxLayout.X_AXIS);
    bxleft.add(boption);
    bxleft.add(Box.createHorizontalGlue());
    bxleft.add(breset);
    bxleft.add(Box.createHorizontalStrut(14));
//  bxleft.add(Box.createRigidArea(new Dimension(5,0)));

    bdown[0].add(Box.createVerticalGlue());
    bdown[0].add(bxleft);

    Runnable constructInSeqAttr = new Runnable()
    {
      public void run () 
      {
//input attributes
        inSeqAttr[h] = new InputSequenceAttributes(db,fileChoose);
        final JScrollPane rscroll = inSeqAttr[h].getJScrollPane();

//cut 'n paste
        cutnPaste = new CutNPasteTextArea(bdown[1],"Sequence Cut and Paste");
 
//buttons for input attributes
        boption.addActionListener(new ActionListener()
        {
          public void actionPerformed(ActionEvent e)
          {
            JOptionPane.showMessageDialog(sectionPane,rscroll,
                    appName.toLowerCase() + " - Input Sequence",
                    JOptionPane.PLAIN_MESSAGE);
          }
        });

        breset.addActionListener(new ActionListener()
        {
          public void actionPerformed(ActionEvent e)
          {
            doReset(); 
          }
        });

        JButton bopt = new JButton("Input Sequence Options");
        JButton bres = new JButton("Reset");
        bopt.addActionListener(new ActionListener()
        {
          public void actionPerformed(ActionEvent e)
          {
            JOptionPane.showMessageDialog(sectionPane,rscroll,
                appName.toLowerCase() + " - Input Sequence",
                JOptionPane.PLAIN_MESSAGE);
          }
        });

        bres.addActionListener(new ActionListener()
        {
          public void actionPerformed(ActionEvent e)
          {
            doReset(); 
          }
        });

        Box bleft= new Box(BoxLayout.X_AXIS);
        bleft.add(bopt);
        bleft.add(Box.createHorizontalGlue());
        bleft.add(bres);
        bleft.add(Box.createHorizontalStrut(5));

        bdown[1].add(bleft);

      }
    };
    SwingUtilities.invokeLater(constructInSeqAttr);

    pfile.add(bdown[0], "File");
    pfile.add(bdown[1], "Paste");
    pfile.add(bdown[2], "List");

    bxleft= new Box(BoxLayout.X_AXIS);
    bxleft.add(pfile);
    bxleft.add(Box.createHorizontalGlue());
    boxFile.add(bxleft);

  }


  protected Box getInCard()
  {
    return boxFile;
  }

/**
*
* @param isFile true if selected to use a file name
*
*/
  public boolean isFileName()
  {
    return isFile;
  }

/**
*
* @param isCut true if selected to use cut 'n paste text
*
*/
  public boolean isCutNPase()
  {
    return isCut;
  }

/**
*
* @param isList true if selected to use list of filenames
*
*/
  public boolean isListFile()
  {
    return isList;
  }

/**
*
* @param String file or database name
*
*/
  public String getFileChosen()
  {
    return fileChoose.getFileChosen();
  }

/**
*
* @param String cut 'n pasted text
*
*/
  public String getCutNPasteText()
  {
    return cutnPaste.getText();
  }

/**
*
* @param String list of sequence filenames
*
*/
  public String getListFile()
  {
    return listPane.getListFile();
  }

/**
*
* @param String[] list of sequence filenames
*
*/
  public String[] getArrayListFile()
  {
    return listPane.getArrayListFile();
  }

/**
*
* @param String list of sequence filename
*
*/
  public String getSequence(int n)
  {
    return listPane.getSequence(n);
  }

  public void doReset()
  {
    cutnPaste.setText("");
    fileChoose.setText("");
    inSeqAttr[h].setBegSeq("");
    inSeqAttr[h].setEndSeq("");
    if(listPane!=null) 
      listPane.doReset();
  }

}
