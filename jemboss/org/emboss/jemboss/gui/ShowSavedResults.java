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
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.tree.*;

import java.awt.event.*;
import java.io.*;
import java.util.*;

import org.emboss.jemboss.programs.*;
import org.emboss.jemboss.soap.*;
import org.emboss.jemboss.JembossParams;

/**
*
* Shows a list of results from the SOAP server
* and displays individual result sets
*
*/
public class ShowSavedResults
{


  private Cursor cbusy = new Cursor(Cursor.WAIT_CURSOR);
  private Cursor cdone = new Cursor(Cursor.DEFAULT_CURSOR);
  private DefaultListModel datasets = new DefaultListModel();
  private JFrame savedResFrame;
  private JPanel sp = new JPanel();
  private JTextArea aboutRes; 
  private JScrollPane aboutScroll;
  private JScrollPane ss;
  private JPanel resButtonStatus;
  private JTextField statusField;
  private JMenuBar resMenu = new JMenuBar();
  private ImageIcon rfii;

  public ShowSavedResults(String frameName)
  {
    savedResFrame = new JFrame(frameName);
    aboutRes = new JTextArea("Select a result set from"
                              +"\nthose listed and details"
                              +"\nof that analysis will be"
                              +"\nshown here. Then you can"
                              +"\neither delete or view those"
                              +"\nresults using the buttons below.");
    aboutScroll = new JScrollPane(aboutRes);  
    ss = new JScrollPane(sp);
    resMenu.setLayout(new FlowLayout(FlowLayout.LEFT,10,1));
    ClassLoader cl = this.getClass().getClassLoader();
    rfii = new ImageIcon(cl.getResource("images/Refresh_button.gif"));

//results status
    resButtonStatus = new JPanel(new BorderLayout());
    Border loweredbevel = BorderFactory.createLoweredBevelBorder();
    Border raisedbevel = BorderFactory.createRaisedBevelBorder();
    Border compound = BorderFactory.createCompoundBorder(raisedbevel,loweredbevel);
    statusField = new JTextField();
    statusField.setBorder(compound);
    statusField.setEditable(false);
  }


/**
*
* Show the saved results on the server.
*
*/
  public ShowSavedResults(final JembossParams mysettings, final JFrame f) 
  {

    this("Saved Results on Server");
     
    Dimension d = new Dimension(270,270);
    ss.setPreferredSize(d);
    ss.setMaximumSize(d);

    try
    {
      final ResultList reslist = new ResultList(mysettings);

      JMenu resFileMenu = new JMenu("File");
      resMenu.add(resFileMenu);

      final JCheckBoxMenuItem listByProgram = 
                 new JCheckBoxMenuItem("List by program");
      listByProgram.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          listByProgramName();
        }
      });
      resFileMenu.add(listByProgram);

      JCheckBoxMenuItem listByDate = 
                 new JCheckBoxMenuItem("List by date",true);
      listByDate.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          listByDateRun(reslist,false);
        }
      });
      resFileMenu.add(listByDate);

      ButtonGroup group = new ButtonGroup();
      group.add(listByProgram);
      group.add(listByDate);

      JButton refresh = new JButton(rfii);
      refresh.setMargin(new Insets(0,1,0,1));
      refresh.setToolTipText("Refresh");
      resMenu.add(refresh);

      refresh.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
        {
          try
          {
            savedResFrame.setCursor(cbusy);
            ResultList newlist = new ResultList(mysettings);
            savedResFrame.setCursor(cdone);
            if (newlist.getStatus().equals("0")) 
            {
              reslist.updateRes(newlist.hash());
              datasets.removeAllElements();

              StringTokenizer tok = new StringTokenizer(
                         (String)reslist.get("list"), "\n");
              while (tok.hasMoreTokens())
                datasets.addElement(tok.nextToken());

              if(listByProgram.isSelected())
                listByProgramName();
              else
                listByDateRun(reslist,false);
            } 
            else 
            {
              JOptionPane.showMessageDialog(savedResFrame,
                     newlist.getStatusMsg(), "Soap Error",
                              JOptionPane.ERROR_MESSAGE);
            }
          } 
          catch (JembossSoapException eae) 
          {
            AuthPopup ap = new AuthPopup(mysettings,f);
            ap.setBottomPanel();
            ap.setSize(380,170);
            ap.pack();
            ap.setVisible(true);
          }
        }
      });


      resFileMenu.addSeparator();
      JMenuItem resFileMenuExit = new JMenuItem("Close");
      resFileMenuExit.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_C, ActionEvent.CTRL_MASK));

      resFileMenuExit.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
        {
          savedResFrame.setVisible(false);
        }
      });
      resFileMenu.add(resFileMenuExit);
      savedResFrame.setJMenuBar(resMenu);
        
      // this is the list of saved results
      listByDateRun(reslist,true);
  
      final JList st = new JList(datasets);
      st.addListSelectionListener(new ListSelectionListener()
      {
        public void valueChanged(ListSelectionEvent e) 
        {
          if (e.getValueIsAdjusting())
            return;

          JList theList = (JList)e.getSource();
          if (theList.isSelectionEmpty()) 
          {
            System.out.println("Empty selection");
          } 
          else 
          {
            int index = theList.getSelectedIndex();
            String thisdata = datasets.elementAt(index).toString();

            aboutRes.setText((String)reslist.get(thisdata));
            aboutRes.setCaretPosition(0);
          }
        }
      });

      st.addMouseListener(new MouseAdapter() 
      {
        public void mouseClicked(MouseEvent e) 
        {
          if (e.getClickCount() == 2) 
          {
            try
            {
              savedResFrame.setCursor(cbusy);
              ResultList thisres = new ResultList(mysettings,(String)st.getSelectedValue(),
                                                      "show_saved_results");
              new ShowResultSet(thisres.hash());
              savedResFrame.setCursor(cdone);
            } 
            catch (JembossSoapException eae) 
            {  
              AuthPopup ap = new AuthPopup(mysettings,f);
              ap.setBottomPanel();
              ap.setSize(380,170);
              ap.pack();
              ap.setVisible(true);
            }
          }
        }
      });
      sp.add(st);
        
      // display retrieves all files and shows them in a window
      JPanel resButtonPanel = new JPanel();
      JButton showResButton = new JButton("Display");
      showResButton.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
	{
          String sel = (String)st.getSelectedValue();
  	  if(sel != null)
          {
	    try 
	    {
	      savedResFrame.setCursor(cbusy);
	      ResultList thisres = new ResultList(mysettings,sel,"show_saved_results"); 
              new ShowResultSet(thisres.hash());
	      savedResFrame.setCursor(cdone);
	    } 
            catch (JembossSoapException eae)
            {
              AuthPopup ap = new AuthPopup(mysettings,f);
              ap.setBottomPanel();
              ap.setSize(380,170);
              ap.pack();
              ap.setVisible(true);
	    }
  	  } 
  	  else 
  	  {
            statusField.setText("Nothing selected to be displayed.");
  	  }
        }
      });
        
      // delete removes the file on the server & edits the list
      JButton delResButton = new JButton("Delete");
      delResButton.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e) 
        {
          Object sel[] = st.getSelectedValues();
          if(sel != null) 
          {
            String selList= new String("");
            for(int i=0;i<sel.length;i++)
              selList=selList.concat(sel[i]+"\n");

            int ok = JOptionPane.OK_OPTION;
            if(sel.length>1)
              ok = JOptionPane.showConfirmDialog(savedResFrame, 
                  "Delete the following results:\n"+selList,
                  "Confirm Deletion", 
                  JOptionPane.YES_NO_OPTION);
            
            if(ok == JOptionPane.OK_OPTION)
            {
              try        // ask the server to delete these results
	      {
	        savedResFrame.setCursor(cbusy); 
	        ResultList thisres = new ResultList(mysettings,selList,
                                         "delete_saved_results"); 
	        savedResFrame.setCursor(cdone);
	       
                // amend the list
                for(int i=0;i<sel.length;i++)
                  datasets.removeElement(sel[i]);
                
                statusField.setText("Deleted " + sel.length + "  result(s)");
     
	        aboutRes.setText("");
	        st.setSelectedIndex(-1);
	      } 
	      catch (JembossSoapException eae) 
	      {
                AuthPopup ap = new AuthPopup(mysettings,f);
                ap.setBottomPanel();
                ap.setSize(380,170);
                ap.pack();
                ap.setVisible(true);
              }
            }
	  } 
          else 
          {
            statusField.setText("Nothing selected to be deleted.");
	  }
	}
      });
      resButtonPanel.add(delResButton);
      resButtonPanel.add(showResButton);
      resButtonStatus.add(resButtonPanel, BorderLayout.CENTER);
      resButtonStatus.add(statusField, BorderLayout.SOUTH);
      savedResFrame.getContentPane().add(ss,BorderLayout.WEST);
      savedResFrame.getContentPane().add(aboutScroll,BorderLayout.CENTER);
      savedResFrame.getContentPane().add(resButtonStatus,BorderLayout.SOUTH);
      savedResFrame.pack();
      
      savedResFrame.setVisible(true);
    } 
    catch (JembossSoapException eae) 
    {
      AuthPopup ap = new AuthPopup(mysettings,f);
      ap.setBottomPanel();
      ap.setSize(380,170);
      ap.pack();
      ap.setVisible(true);
    }

  }


/**
*
* Show the results sent to a batch queue.
*
*/
  public ShowSavedResults(final JembossParams mysettings, final PendingResults epr)
                                           throws JembossSoapException
  {

    this("Current Sessions Results");

    Dimension d = new Dimension(270,100);
    ss.setPreferredSize(d);
    ss.setMaximumSize(d);

    JMenu resFileMenu = new JMenu("File");
    resMenu.add(resFileMenu);

    JButton refresh = new JButton(rfii);
    refresh.setMargin(new Insets(0,1,0,1));
    refresh.setToolTipText("Refresh");
    resMenu.add(refresh);
    refresh.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
	savedResFrame.setCursor(cbusy);
	epr.updateStatus();
	savedResFrame.setCursor(cdone);
	datasets.removeAllElements();
	Enumeration enum = epr.descriptionHash().keys();
	while (enum.hasMoreElements()) 
        {
	  String image = (String)enum.nextElement().toString();
	  datasets.addElement(image);
	}
      }
    });

    JMenuItem resFileMenuExit = new JMenuItem("Close",KeyEvent.VK_C);
    resFileMenuExit.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_C, ActionEvent.CTRL_MASK));

    resFileMenuExit.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
	savedResFrame.dispose();
      }
    });
    resFileMenu.add(resFileMenuExit);
    savedResFrame.setJMenuBar(resMenu);
    
    // set up the results list in the gui
    Enumeration enum = epr.descriptionHash().keys();
    while (enum.hasMoreElements()) 
    {
      String image = (String)enum.nextElement().toString();
      datasets.addElement(image);
    }

    final JList st = new JList(datasets);
    st.addListSelectionListener(new ListSelectionListener()
    {
      public void valueChanged(ListSelectionEvent e) 
      {
	if (e.getValueIsAdjusting())
	  return;
	
	JList theList = (JList)e.getSource();
	if (!theList.isSelectionEmpty()) 
        {
	  int index = theList.getSelectedIndex();
	  String thisdata = datasets.elementAt(index).toString();
	  aboutRes.setText((String)epr.descriptionHash().get(thisdata));
      	  aboutRes.setCaretPosition(0);
	  aboutRes.setEditable(false);
	}
      }
    });


    st.addMouseListener(new MouseAdapter() 
    {
      public void mouseClicked(MouseEvent e) 
      {
	if (e.getClickCount() == 2) 
        {
	  try
          {
	    savedResFrame.setCursor(cbusy);
	    ResultList thisres = new ResultList(mysettings, (String)st.getSelectedValue(), 
                                                     "show_saved_results");
	    savedResFrame.setCursor(cdone);
	    if (thisres.getStatus().equals("0")) 
              new ShowResultSet(thisres.hash());
            else 
              JOptionPane.showMessageDialog(savedResFrame,
                     thisres.getStatusMsg(), "Soap Error",
                              JOptionPane.ERROR_MESSAGE);
 
	  } 
          catch (JembossSoapException eae) 
          {
            AuthPopup ap = new AuthPopup(mysettings,savedResFrame);
            ap.setBottomPanel();
            ap.setSize(380,170);
            ap.pack();
            ap.setVisible(true);
	  }
	}
      }
    });
    sp.add(st);
    
    // display retrieves all the files and shows them in a window
   
    JPanel resButtonPanel = new JPanel();
    JButton showResButton = new JButton("Display");
    showResButton.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e) 
      {
	if(st.getSelectedValue() != null) 
        {
	  try
          {
	    savedResFrame.setCursor(cbusy);
	    ResultList thisres = new ResultList(mysettings, (String)st.getSelectedValue(), 
                                                      "show_saved_results");
	    savedResFrame.setCursor(cdone);
	    if (thisres.getStatus().equals("0")) 
              new ShowResultSet(thisres.hash());
            else 
              JOptionPane.showMessageDialog(savedResFrame,
                     thisres.getStatusMsg(), "Soap Error",
                              JOptionPane.ERROR_MESSAGE);
	  } 
          catch (JembossSoapException eae) 
          {
            savedResFrame.setCursor(cdone);
            AuthPopup ap = new AuthPopup(mysettings,savedResFrame);
            ap.setBottomPanel();
            ap.setSize(380,170);
            ap.pack();
            ap.setVisible(true);
	  }
	}
      }
    });
    
    // delete removes the file on the server and edits the list
   
    JButton delResButton = new JButton("Delete");
    delResButton.addActionListener(new ActionListener()
    {
      public void actionPerformed(ActionEvent e) 
      {
        Object sel[] = st.getSelectedValues();
	if(sel != null) 
        {
          String selList= new String("");
          for(int i=0;i<sel.length;i++)
            selList=selList.concat(sel[i]+"\n");

          int ok = JOptionPane.OK_OPTION;
          if(sel.length>1)
            ok = JOptionPane.showConfirmDialog(savedResFrame,
                "Delete the following results:\n"+selList,
                "Confirm Deletion",
                JOptionPane.YES_NO_OPTION);
           
          if(ok == JOptionPane.OK_OPTION)
          {

	    try 
            {
	      savedResFrame.setCursor(cbusy);
	      ResultList thisres = new ResultList(mysettings,selList,
                                             "delete_saved_results");
	      savedResFrame.setCursor(cdone);

              for(int i=0;i<sel.length;i++)
              {
                JembossProcess jp = epr.getResult((String)sel[i]);
                epr.removeResult(jp);
                datasets.removeElement(sel[i]);  // amend the list
              }
              statusField.setText("Deleted " + sel.length+ "  result(s)");
	      aboutRes.setText("");
	      st.setSelectedIndex(-1);
	    }
            catch (JembossSoapException eae)
            {
	      // shouldn't happen
              AuthPopup ap = new AuthPopup(mysettings,savedResFrame);
              ap.setBottomPanel();
              ap.setSize(380,170);
              ap.pack();
              ap.setVisible(true);
	    }
          }
	}
      }
    });
    resButtonPanel.add(delResButton);
    resButtonPanel.add(showResButton);
    resButtonStatus.add(resButtonPanel, BorderLayout.CENTER);
    resButtonStatus.add(statusField, BorderLayout.SOUTH);

    savedResFrame.getContentPane().add(ss,BorderLayout.WEST);
    savedResFrame.getContentPane().add(aboutScroll,BorderLayout.CENTER);
    savedResFrame.getContentPane().add(resButtonStatus,BorderLayout.SOUTH);
    savedResFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
    
    savedResFrame.pack();
    savedResFrame.setVisible(true);

//add in automatic updates
    String freq = (String)AdvancedOptions.jobMgr.getSelectedItem();
    int ind = freq.indexOf(" ");
    new ResultsUpdateTimer(Integer.parseInt(freq.substring(0,ind)),
                           datasets, savedResFrame);
    statusField.setText("Window refresh rate " + freq);

  }


  private void listByDateRun(ResultList reslist,boolean ldisp)
  {
    StringTokenizer tokenizer =
         new StringTokenizer((String)reslist.get("list"), "\n");

    Vector vdata = new Vector();
    while (tokenizer.hasMoreTokens())
    {
      String data = tokenizer.nextToken();
      if(datasets.contains(data) || ldisp)
        vdata.add(data);
    }
    datasets.removeAllElements();

    Enumeration en = vdata.elements();
    while(en.hasMoreElements())
      datasets.addElement(en.nextElement());

  }


  private void listByProgramName()
  { 
    int nresult = datasets.size();
    String res[] = new String[nresult];
    for(int i=0;i<nresult;i++)
      res[i] = (String)datasets.getElementAt(i);
    Arrays.sort(res);
    datasets.removeAllElements();
    for(int i=0;i<nresult;i++)
      datasets.addElement(res[i]);
  }

}

