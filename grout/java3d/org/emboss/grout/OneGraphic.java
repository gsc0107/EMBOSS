/***************************************************************
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *  @author: Copyright (C) Hugh Morgan
 *
 ***************************************************************/

/*
 * OneGraphic.java
 *
 * Created on Feb 3 2003
 */

package org.emboss.grout;

import java.awt.GridBagLayout;
import java.util.Vector;
import java.util.Enumeration;
import java.awt.image.BufferedImage;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
//import ;
//import ;

// the ones from MiniFrame
import java.beans.*;
import javax.swing.*;

// the ones from loader
// Standard imports
import java.applet.Applet;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.media.j3d.*;
import javax.vecmath.*;
import java.net.URL;
import java.net.MalformedURLException;

// Application specific imports
import com.sun.j3d.utils.applet.MainFrame;
import com.sun.j3d.utils.universe.*;
import com.sun.j3d.utils.behaviors.mouse.MouseRotate;
import com.sun.j3d.utils.behaviors.mouse.MouseZoom;
import com.sun.j3d.utils.behaviors.mouse.MouseTranslate;

import com.sun.j3d.loaders.Scene;

import org.ietf.uri.ContentHandlerFactory;
import org.ietf.uri.FileNameMap;
import org.ietf.uri.URI;

import org.web3d.j3d.loaders.VRML97Loader;
import org.web3d.net.content.VRMLContentHandlerFactory;
import org.web3d.net.content.VRMLFileNameMap;

// Picking imports

import com.sun.j3d.utils.picking.*;


/**
 * A Panel to display One Graphic
 *
 * @author  hmorgan2
 */
public class OneGraphic extends javax.swing.JPanel implements
java.io.Serializable, ParserListener, GroutPanelListener,
ViewPositionModelListener
{
    private boolean offScreen = false;
    /** true if all the graphics have been removed and this is displayed as a white box */    
    private boolean thisIsWhite = false;
    private int graphicNumber = 0;
    
    private RendWindContainer canvas;
    private JPanel xAxis;
    private AxisMarks xAxisMarks;
    // private SequenceView xSequence;
    private AxisMarks yAxisMarks;
    // private SequenceView ySequence;
    //private JLabel mainTitle;
    private LabelWithVerticalText mainTitle;
    // private JLabel xTitle;
    private LabelWithVerticalText xTitle;
    private LabelWithVerticalText yTitle;
    //    private JLabel yTitle;
    //    private JEditorPane yTitle;
    
    private Font mainTitleFont;
    private Font xTitleFont;
    private Font yTitleFont;
    
    private ViewPositionModel viewPositionModel;
    private ControlContainer controls;
    
    private Metadata metadata = null;
    
    private GridBagLayout gridbag = null;
    private GridBagConstraints c = null;
    
    private Vector scenes = new Vector();
    
    private TransferableGraphic originalGraphic = null;
    
    private GroutPanel parentPanel = null;
    private PositionLabel positionLabel = null;
    /** box to make the display the right colour */    
    private	JPanel blankGrayPanel;
    /** box to make the display the right colour */    
    private	JPanel blankWhitePanel;
    /** box to make the display the right colour */    
    private JPanel empty;
    /** box to make the display the right colour */    
    private JPanel empty2;
    
    private Vector targets = new Vector();
    
    /**
     * Standard NULL constructor
     *
     */
    public OneGraphic()
    {
        super();
        doSetup();
        // System.out.println("this = " + this);
        originalGraphic = new TransferableGraphic(null, this, this, 0, graphicNumber, viewPositionModel, metadata);
        scenes.addElement(originalGraphic);
        return;
    }
    
    /** constructor that sets the parent
     * @param parent parent
     */
    public OneGraphic(Component parent)
    {
        super();
        doSetup();
        
        // System.out.println("parent = " + parent);
        // if(parent.getClass().isAssignableFrom(GroutPanel.class))
        // System.out.println("parent = " + parent);
        parentPanel = ((GroutPanel) parent);
        parentPanel.addGroutPanelListener(viewPositionModel);
        // System.out.println("this = " + this);
        // System.out.println("parentPanel = " + parentPanel);
        // System.out.println("originalGraphic = " + originalGraphic);
        originalGraphic = new TransferableGraphic(null, this, this, 0,
        graphicNumber,
        viewPositionModel,
        metadata);
        scenes.addElement(originalGraphic);
        
        return;
    }
    
    private RendWindContainer getCanvas()
    {
        return canvas;
    }
    
    public String getMainTitle()
    {
        // System.out.println("xTitle = " + xTitle);
        return mainTitle.getText();
    }
    
    public String getXTitle()
    {
        // System.out.println("xTitle = " + xTitle);
        return xTitle.getText();
    }
    
    public String getYTitle()
    {
        // System.out.println("yTitle = " + yTitle);
        return yTitle.getText();
    }
    
    public void setMainTitle(String title)
    {
        // System.out.println("xTitle = " + xTitle);
        mainTitle.setText(title);
    }
    
    public void setXTitle(String title)
    {
        // System.out.println("xTitle = " + xTitle);
        xTitle.setText(title);
    }
    
    public void setYTitle(String title)
    {
        // System.out.println("yTitle = " + yTitle);
        yTitle.setText(title);
    }
    
    public ControlContainer getControls()
    {
        return controls;
    }
    
    public Vector getScenes()
    {
        return scenes;
    }
    
    public void setLocationListener(DragableCanvas3DListener lis)
    {
        canvas.setLocationListener(lis);
    }
    
    /** does the laying out of the panel */    
    private void doSetup()
    {
        gridbag = new GridBagLayout();
        c = new GridBagConstraints();
        
        setLayout(gridbag);
        
        viewPositionModel = new ViewPositionModel();
        controls = new ControlContainer();
        
        canvas = new RendWindContainer(offScreen);
        canvas.setGraphicNumber(graphicNumber);
        // canvas.setLocationListener(parentPanel.getLocationListener());
        
        xAxis = new JPanel(new BorderLayout());
        xAxisMarks = new AxisMarks(offScreen);
        xAxisMarks.addMouseListener(new LabelListener(xAxisMarks, null,
        "AxisMarks"));
        // xSequence = new SequenceView();
        yAxisMarks = new AxisMarks(offScreen);
        yAxisMarks.addMouseListener(new LabelListener(yAxisMarks, null,
        "AxisMarks"));
                                /*
                                positionLabel = new PositionLabel();
                                canvas.setLocationListener(positionLabel);
                                 */
        
        // ySequence = new SequenceView();
        
        //				setBackground(Color.black);
        setBackground(Color.white);
        
        c.gridx = 0;
        c.gridy = 1;
        c.weightx = 0.0;
        c.weighty = 0.0;
        c.gridheight = 1;
        c.fill = GridBagConstraints.VERTICAL;
        gridbag.setConstraints(controls.getCentrePanel(), c);
        add(controls.getCentrePanel());
        c.gridheight = 1;
        
        c.gridx = 0;
        c.gridy = 0;
        c.weightx = 0.0;
        c.weighty = 0.0;
        c.gridheight = 1;
        c.fill = GridBagConstraints.BOTH;
        empty = new JPanel();
        gridbag.setConstraints(empty, c);
        add(empty);
        c.gridheight = 1;
        
        c.gridx = 0;
        c.gridy = 2;
        c.weightx = 0.0;
        c.weighty = 0.0;
        c.gridheight = 3;
        c.fill = GridBagConstraints.BOTH;
        empty2 = new JPanel();
        gridbag.setConstraints(empty2, c);
        add(empty2);
        c.gridheight = 1;
        
        c.gridx = 4;
        c.gridy = 5;
        c.gridwidth = 1;
        c.gridheight = 2;
        c.fill = GridBagConstraints.BOTH;
        // c.insets.right = GroutPanel.RIGHT_INDENT;
        //				c.insets.right = 5;
        gridbag.setConstraints(controls.getBottomPanel(), c);
        add(controls.getBottomPanel());
        //				c.insets.right = 0;
        c.insets.right = 0;
        
        c.gridx = 0;
        c.gridy = 5;
        c.gridwidth = 4;
        c.gridheight = 2;
        c.fill = GridBagConstraints.BOTH;
        gridbag.setConstraints(controls.getLockButtonPanel(), c);
        add(controls.getLockButtonPanel());
        c.gridwidth = 1;
        c.gridheight = 1;
        
        c.gridx = 4;
        c.gridy = 1;
        c.weightx = 1.0;
        c.weighty = 1.0;
        c.fill = GridBagConstraints.BOTH;
        // c.insets.right = GroutPanel.RIGHT_INDENT;
        gridbag.setConstraints(canvas, c);
        add(canvas);
        c.weightx = 0.0;
        c.weighty = 0.0;
        
        c.gridx = 4;
        c.gridy = 2;
        c.fill = GridBagConstraints.HORIZONTAL;
        gridbag.setConstraints(xAxisMarks, c);
        add(xAxisMarks);
        c.insets.right = 0;
        
        
        yAxisMarks.setOrientation(AxisMarks.VERTICAL);
        c.gridx = 3;
        c.gridy = 1;
        c.fill = GridBagConstraints.VERTICAL;
        gridbag.setConstraints(yAxisMarks, c);
        add(yAxisMarks);
        
                                /*
                                c.gridx = 0;
                                c.gridy = 2;
                                c.gridwidth = 4;
                                c.gridheight = 2;
                                c.fill = GridBagConstraints.HORIZONTAL;
                                gridbag.setConstraints(positionLabel, c);
                                c.anchor = GridBagConstraints.WEST;
                                add(positionLabel);
                                c.gridwidth = 1;
                                c.gridheight = 1;
                                 */
        
        c.gridx = 5;
        c.gridy = 0;
        c.gridwidth = 1;
        c.gridheight = 5;
        c.fill = GridBagConstraints.BOTH;
        blankWhitePanel = (new JPanel()
        {
            public void paint(Graphics g)
            {
                g.setColor(Color.white);
                g.fillRect(0, 0, getSize().width, getSize().height);
            }
        });
        blankWhitePanel.setSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
        blankWhitePanel.setMinimumSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
        blankWhitePanel.setMaximumSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
        
        blankWhitePanel.setPreferredSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
                
        gridbag.setConstraints(blankWhitePanel, c);
        add(blankWhitePanel);
        c.gridwidth = 1;
        c.gridheight = 1;
        
        c.gridx = 5;
        c.gridy = 4;
        c.gridwidth = 1;
        c.gridheight = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.BOTH;
        blankGrayPanel =  new JPanel();
        blankGrayPanel.setPreferredSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
        blankGrayPanel.setSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
        blankGrayPanel.setMinimumSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
        blankGrayPanel.setMaximumSize
        (new Dimension(GroutPanel.RIGHT_INDENT, 1));
        
        gridbag.setConstraints(blankGrayPanel, c);
        add(blankGrayPanel);
        c.gridwidth = 1;
        c.gridheight = 1;
        
        //		addMainTitle("Title");
        
        setInternalListeners();
        
        return;
    }
    
    public void addMainTitle(String title)
    {
        // System.out.println("addMainTitle = " + title);
        
        // mainTitle = new JLabel(title);
        mainTitle = new LabelWithVerticalText(title);
        mainTitle.setOrientation(LabelWithVerticalText.HORIZONTAL);
        mainTitle.addMouseListener(new LabelListener(null, this, "MainTitle"));
        mainTitle.setPreferredSize(new Dimension(400, 30));
        //				mainTitle.setForeground(Color.WHITE);
        mainTitle.setForeground(Color.black);
        mainTitleFont = new Font("TimesRoman",Font.PLAIN,14);
        mainTitle.setFont(mainTitleFont);
        mainTitle.setHorizontalAlignment(SwingConstants.CENTER);
        //				mainTitle.add(text);
        c.gridx = 4;
        c.gridy = 0;
        c.fill = GridBagConstraints.HORIZONTAL;
        gridbag.setConstraints(mainTitle, c);
        if(!isAncestorOf(mainTitle))
        {
            add(mainTitle);
        }
    }
    
    public void addXTitle(String title)
    {
        // xTitle = new JLabel(title);
        xTitle = new LabelWithVerticalText(title);
        xTitle.setOrientation(LabelWithVerticalText.HORIZONTAL);
        xTitle.addMouseListener(new LabelListener(null, this, "XTitle"));
        xTitle.setPreferredSize(new Dimension(400, 20));
        xTitle.setMinimumSize(new Dimension(400, 20));
        //				xTitle.setForeground(Color.WHITE);
        xTitle.setBackground(Color.white);
        xTitle.setForeground(Color.black);
        xTitleFont = new Font("TimesRoman",Font.PLAIN,12);
        xTitle.setFont(xTitleFont);
        xTitle.setHorizontalAlignment(SwingConstants.CENTER);
        c.gridx = 4;
        c.gridy = 4;
        c.fill = GridBagConstraints.HORIZONTAL;
        gridbag.setConstraints(xTitle, c);
        if(!isAncestorOf(xTitle))
        {
            add(xTitle);
        }
        repaint();
    }
    
    public void addYTitle(String title)
    {
        yTitle = new LabelWithVerticalText(title);
        yTitle.addMouseListener(new LabelListener(null, this, "YTitle"));
        //			  yTitle = new JLabel(verticalTitle);
        //			  yTitle = new JEditorPane();
        //			  yTitle.setText(title);
        yTitle.setPreferredSize(new Dimension(15, 400));
        // I do not know why this line is needed, but else you lose the axis
        yTitle.setMinimumSize(new Dimension(15, 400));
        //				yTitle.setForeground(Color.WHITE);
        yTitle.setBackground(Color.white);
        yTitle.setForeground(Color.black);
        yTitleFont = new Font("TimesRoman",Font.PLAIN,12);
        yTitle.setFont(yTitleFont);
        c.gridx = 1;
        c.gridy = 1;
        c.fill = GridBagConstraints.VERTICAL;
        gridbag.setConstraints(yTitle, c);
        if(!isAncestorOf(yTitle))
        {
            add(yTitle);
        }
        repaint();
    }
    
    public void setYAxisMarks(String presentString)
    {
        try
        {
            boolean present = Boolean.valueOf(presentString).booleanValue();
            yAxisMarks.setDrawAxis(present);
        }
        catch(NumberFormatException e)
        {
            e.printStackTrace();
        }
    }
    
    
    /**
     *  Constructor for offscreen instance
     *
     * @param os True for offscreen instance
     */
    public OneGraphic(boolean os)
    {
        super();
        offScreen = os;
        doSetup();
        return;
    }
    
    /**
     *  Sets the graphic number that this displays
     *
     * @param no the graphic number
     */
    public void setGraphicNumber(int no)
    {
        graphicNumber = no;
        canvas.setGraphicNumber(graphicNumber);
        viewPositionModel.setGraphicNumber(graphicNumber);
        return;
    }
    
    /**
     *  Gets the graphic number that this displays
     *
     * @return int the graphic number
     */
    public int getGraphicNumber()
    {
        return graphicNumber;
    }
    
    public RendWindContainer getRendWindContainer()
    {
        return canvas;
    }
    
    /**
     *  Gets the size of the rendering window
     *
     * @return int the size of the rendering window
     */
    public int getRendWindContainerSize()
    {
        // System.out.println("getWidth = " + canvas.getCanvas().getWidth());
        return canvas.getCanvas().getWidth();
    }
    
    public ViewPositionModel getViewPositionModel()
    {
        return viewPositionModel;
    }
    
    public Metadata getMetadata()
    {
        return metadata;
    }
    
    public TransferableGraphic getOriginalGraphic()
    {
        return originalGraphic;
    }
    
    /**
     *  Sets up the internal Listeners
     *
     */
    private void setInternalListeners()
    {
        viewPositionModel.addViewPositionModelListener(xAxisMarks);
        viewPositionModel.addViewPositionModelListener(yAxisMarks);
        viewPositionModel.addViewPositionModelListener(canvas);
        viewPositionModel.addViewPositionModelListener(controls);
        viewPositionModel.addViewPositionModelListener(this);
        
        controls.addControlContainerListener(viewPositionModel);
        
        canvas.addRendWindContainerListener(viewPositionModel);
        
        //		viewPositionModel.addViewPositionModelListener(xSequence);
        //		viewPositionModel.addViewPositionModelListener(ySequence);
        
        return;
    }
    
    /**
     *  Sets up the parser Listeners
     *
     * @param parser the parser
     */
    public void setParserListeners(Parser parser)
    {
        parser.addParserListener(canvas);
        parser.addParserListener(viewPositionModel);
        parser.addParserListener(this);
        
        return;
    }
    
    /**
     *  Sets up the GroutPanel Listeners
     *
     * @param GroutPanel the GroutPanel
     */
    public void setGroutPanelListeners(GroutPanel panel)
    {
        panel.addGroutPanelListener(canvas);
        panel.addGroutPanelListener(xAxisMarks);
        panel.addGroutPanelListener(yAxisMarks);
        
        return;
    }
    
    /**
     *  Sets up the ControlContainer Listeners
     *
     * @param controlContainer the ControlContainer
     */
    public void setControlListeners(ControlContainer controlContainer)
    {
        controlContainer.addControlContainerListener(viewPositionModel);
        viewPositionModel.addViewPositionModelListener(controlContainer);
        
        return;
    }
    
    public void	setMetadata(Metadata md)
    {
        // System.out.println("setMetadata addMainTitle");
        metadata = md;
        setTitles();
        originalGraphic.setMetadata(metadata);
    }
    
    public void	setTitles()
    {
        boolean doneKey = false;
        // System.out.println("setTitles addMainTitle");
        for (Enumeration e = metadata.keys() ; e.hasMoreElements() ;)
        {
            String key = (String) e.nextElement();
            if(key.equals("Graph.mainTitle"))
                addMainTitle(metadata.getValue(key));
            if(key.equals("Graph.xTitle"))
                addXTitle(metadata.getValue(key));
            if(key.equals("Graph.yTitle"))
                addYTitle(metadata.getValue(key));
            if(key.equals("Graph.yAxisMarks"))
                setYAxisMarks(metadata.getValue(key));
            if(key.equals("Graph.colour"))
            {
                String shortTitle = metadata.getValue("Graph.shortTitle");
                if(shortTitle != null)
                    addThisGraphicToKey(metadata.getValue(key),
                    metadata.getValue
                    ("Graph.shortTitle"));
                else
                    addThisGraphicToKey(metadata.getValue(key),
                    metadata.getValue
                    ("Graph.mainTitle"));
                // checkLineColour(metadata.getValue(key));
                doneKey = true;
            }
            // add rest of metadata hugh
        }
        if(!doneKey)
        {
            String shortTitle = metadata.getValue("Graph.shortTitle");
            if(shortTitle != null)
                addThisGraphicToKey("0 0 0", shortTitle);
            else
                addThisGraphicToKey("0 0 0",
                metadata.getValue("Graph.mainTitle"));
            
            
        }
    }
    
    private void addThisGraphicToKey(String colour, String title)
    {
        // ((GroutPanel) getParent()).addThisGraphicToKey(colour, title);
        parentPanel.addThisGraphicToKey(colour, title);
    }
    
    /** not sure if I shall use this */    
    private void checkLineColour(final String colour)
    {
        
        if(originalGraphic.getSceneRoot() != null)
        {
            changeLineColour(colour);
        }
        else
        {
            // System.out.println("erty not doing changeLineColour co" + 
						// 									 " originalGraphic.getSceneRoot() = null ");
        }
    }
    
    /** not sure if I shall use this */    
    private void changeLineColour(final String colour)
    {
        BranchGroup sceneRoot = originalGraphic.getSceneRoot();
        Geometry geometry = findGeometry(sceneRoot, colour);
        if(thisGeometryIsALine(geometry))
        {
            changeThisLineColour(colour, geometry);
        }
        
    }
    
    /** not sure if I shall use this */    
    private Geometry findGeometry(BranchGroup sceneRoot, String colour)
    {
        SceneGraphObject node = sceneRoot;
        boolean foundGeometry = false;
        
        while(!(node instanceof Geometry))
        {
            if(node instanceof Node)
            {
                node = getChildOfNode((Node)node, colour);
            }
        }
        return ((Shape3D) node).getGeometry();
    }
    
    /** not sure if I shall use this */    
    private Node getChildOfNode(Node node, String colour)
    {
        if(node instanceof Link)
        {
            SharedGroup thisGroup = ((Link) node).getSharedGroup();
            node = getChildOfNode(node, colour);
        }
        else if(node instanceof Group)
        {
            Node child = null;
            for (Enumeration children = ((Group) node).getAllChildren();
            children.hasMoreElements();
            child = (Node) children.nextElement())
            {
                node = getChildOfNode(child, colour);
            }
        }
        else if(node instanceof Shape3D)
        {
            for(int j = 0; j < (((Shape3D) node).numGeometries()); ++j)
            {
                // System.out.println("child = " + child);
                Geometry geometry = ((Shape3D) node).getGeometry(j);
                if(thisGeometryIsALine(geometry))
                {
                    changeThisLineColour(colour, geometry);
                    return node;
                }
            }
        }
        return node;
    }
    
    /** not sure if I shall use this */    
    private void changeThisLineColour(String colour, Geometry geometry)
    {
        System.out.println("erty changing line colour of " +geometry  
													 + " to " + colour);
    }
    
    /** not sure if I shall use this */    
    private boolean thisGeometryIsALine(Geometry geometry)
    {
        return true;
    }
    
    /** Removes the x axis, cos it has a shared one. */    
    private void removeXAxis()
    {
        // System.out.println("removeXAxis");
        remove(xAxisMarks);
        // remove(xSequence);
        remove(controls.getLockButtonPanel());
        remove(controls.getBottomPanel());
        remove(xTitle);
        // remove(positionLabel);
        remove(blankGrayPanel);
        remove(empty2);
        validate();
    }
    
    private void listenToThisControl(ControlContainer controls)
    {
        //				viewPositionModel.addViewPositionModelListener(controls);
        controls.addControlContainerListener(viewPositionModel);
    }
    
    /** add an x axis because it no longer has a shared axis.  Does not work well */    
    private void addXAxis()
    {
        c.gridx = 3;
        c.gridy = 5;
        c.fill = GridBagConstraints.HORIZONTAL;
        gridbag.setConstraints(controls.getBottomPanel(), c);
        add(controls.getBottomPanel());
        
        c.gridx = 0;
        c.gridy = 5;
        c.gridwidth = 3;
        c.gridheight = 2;
        c.fill = GridBagConstraints.HORIZONTAL;
        gridbag.setConstraints(controls.getLockButtonPanel(), c);
        add(controls.getLockButtonPanel());
        c.gridwidth = 1;
        c.gridheight = 1;
        
        c.gridx = 3;
        c.gridy = 2;
        c.fill = GridBagConstraints.HORIZONTAL;
        gridbag.setConstraints(xAxis, c);
        add(xAxis);
        
        c.gridx = 3;
        c.gridy = 3;
        c.fill = GridBagConstraints.HORIZONTAL;
        gridbag.setConstraints(xTitle, c);
        add(xTitle);
        
        c.gridx = 0;
        c.gridy = 2;
        c.gridwidth = 4;
        c.gridheight = 2;
        c.fill = GridBagConstraints.BOTH;
        gridbag.setConstraints(positionLabel, c);
        add(positionLabel);
        c.gridwidth = 1;
        c.gridheight = 1;
        
        validate();
    }
    
    /** no longer listen to shared x axis */    
    private void listenToOwnControl()
    {
        //				viewPositionModel.removeViewPositionModelListener(controls);
        controls.removeControlContainerListener(viewPositionModel);
    }
    
    public BranchGroup detachScene(int sceneNumber)
    {
        return canvas.detachScene();
    }
    
    public BranchGroup detachScene(BranchGroup branchGroup)
    {
        return canvas.detachScene(branchGroup);
    }
    
    public void addScene(BranchGroup sc)
    {
        canvas.addScene(sc);
    }
    
    public void addScene(BranchGroup sc, OneGraphic cameFrom)
    // private void addScene(BranchGroup sc, OneGraphic cameFrom)
    {
        // System.out.println("I hope this isn't run");
        scenes.addElement(cameFrom);
        canvas.addScene(sc);
    }
    
    public TransferableGraphic getTransferableGraphic(BranchGroup sc)
    {
        // System.out.println("looking for = " + sc);
        // System.out.println("scenes.size() = " + scenes.size());
        
        TransferableGraphic returnGraphic = null;
        for (Enumeration e = scenes.elements(); (e.hasMoreElements() &&
        (returnGraphic == null));)
        {
            TransferableGraphic tempGraphic =
            (TransferableGraphic) e.nextElement();
            if(tempGraphic.getSceneRoot() != null)
            {
                if(tempGraphic.getSceneRoot().equals(sc))
                {
                    returnGraphic = tempGraphic;
                }
            }
        }
        return returnGraphic;
    }
    
    public void addThisGraphic(TransferableGraphic tr)
    {
        scenes.addElement(tr);
        // System.out.println("scenes.size() af = " + scenes.size());
        canvas.addScene(tr.getSceneRoot());
        
        boolean sizeChanged = false;
        
        if(tr.getMaxX() > viewPositionModel.getMaxX())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMaxX(tr.getMaxX());
            // System.out.println("setting tr.getMaxX() to = " + tr.getMaxX());
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(tr.getMinX() < viewPositionModel.getMinX())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMinX(tr.getMinX());
            // System.out.println("setting tr.getMinX() to = " + tr.getMinX());
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(tr.getMaxY() > viewPositionModel.getMaxY())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMaxY(tr.getMaxY());
            // System.out.println("setting tr.getMaxY() to = " + tr.getMaxY());
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(tr.getMinY() < viewPositionModel.getMinY())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMinY(tr.getMinY());
            // System.out.println("setting tr.getMinY() to = " + tr.getMinY());
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(sizeChanged)
        {
            Dimension renderingWindowSize = canvas.getSize();
            viewPositionModel.fitViewToWindow(renderingWindowSize,
            new Dimension(1280, 1024));
        }
        
        addTitle(tr);
        
        // canvas.addSceneScaled(tr);
    }
    
    private void addTitle(TransferableGraphic tr)
    {
        String oldTitle = getMainTitle();
        String newTitle = oldTitle + " and " + tr.getMetadata().getValue("Graph.mainTitle");
        setMainTitle(newTitle);
    }
    
    public void removeThisGraphic(TransferableGraphic tr)
    {
        detachScene(tr.getSceneRoot());
        scenes.remove(tr);
        removeTitle(tr);
        // System.out.println("scenes.size() af = " + scenes.size());
        setViewSize();
    }
    
    private void setViewSize()
    {
        // so, either I calculate the required size from the remaining
        // graphics, or I work out the dimemsions that could have been
        // limited by the one removed, and look at them only. hugh
        
        double maxX = - Double.MAX_VALUE;
        double minX = Double.MAX_VALUE;
        double maxY = - Double.MAX_VALUE;
        double minY = Double.MAX_VALUE;
        boolean sizeChanged = false;
        
        for (Enumeration e = scenes.elements() ; e.hasMoreElements() ;)
        {
            TransferableGraphic tr = (TransferableGraphic) e.nextElement();
            
            if(tr.getMaxX() > maxX)
            {
                maxX = tr.getMaxX();
            }
            if(tr.getMinX() < minX)
            {
                minX = tr.getMinX();
            }
            if(tr.getMaxY() > maxY)
            {
                maxY = tr.getMaxY();
            }
            if(tr.getMinY() < minY)
            {
                minY = tr.getMinY();
            }
        }
        // System.out.print("setting");
        // viewPositionModel.printValues();
        if(maxX < viewPositionModel.getMaxX())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMaxX(maxX);
            // System.out.println("setting maxX() to = " + maxX);
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(minX > viewPositionModel.getMinX())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMinX(minX);
            // // System.out.println("setting minX() to = " + minX);
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(maxY < viewPositionModel.getMaxY())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMaxY(maxY);
            // System.out.println("setting maxY() to = " + maxY);
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(minY > viewPositionModel.getMinY())
        {
            // viewPositionModel.printValues();
            viewPositionModel.setMinY(minY);
            // System.out.println("setting minY() to = " + minY);
            // viewPositionModel.printValues();
            sizeChanged = true;
        }
        if(sizeChanged)
        {
            Dimension renderingWindowSize = canvas.getSize();
            viewPositionModel.fitViewToWindow(renderingWindowSize,
            new Dimension(1280, 1024));
        }
        
    }
    
    
    private void removeTitle(TransferableGraphic tr)
    {
        String oldTitle = getMainTitle();
        String removedTitle = tr.getMetadata().getValue("Graph.mainTitle");
        int positionOfRemovedTitle = oldTitle.indexOf(removedTitle);
        String newTitle = new String(oldTitle);
        if(positionOfRemovedTitle != -1)
        {
            newTitle = oldTitle.substring(0,
            positionOfRemovedTitle)
            + oldTitle.substring(positionOfRemovedTitle +
            removedTitle.length());
            int positionOfAnd = newTitle.indexOf(" and ", positionOfRemovedTitle);
            if(positionOfAnd == -1)
            {
                positionOfAnd = newTitle.lastIndexOf(" and ",
                positionOfRemovedTitle);
            }
            if(positionOfAnd != -1)
            {
                newTitle = newTitle.substring(0,
                positionOfAnd)
                + newTitle.substring(positionOfAnd + 5);
            }
        }
        if(!newTitle.equals(""))
        {
            setMainTitle(newTitle);
        }
    }
    
    public void paint(Graphics g)
    {
        // System.out.println("paint Onegraphic thisIsWhite = " + thisIsWhite);
        if(thisIsWhite)
        {
            // System.out.println("paint Onegraphic white");
            remove(canvas);
            final Dimension originalSize = getSize();
            g.setColor(Color.white);
            g.fillRect(0, 0, originalSize.width, originalSize.height);
        }
        else
        {
            super.paint(g);
        }
    }
    
    public boolean isAWhiteBox()
    {
        return thisIsWhite;
    }
    
    public void makeAWhiteBox()
    {
        // System.out.println("makeAWhiteBox");
        thisIsWhite = true;
        xAxisMarks.setMakeAWhiteBox(true);
        yAxisMarks.setMakeAWhiteBox(true);
        repaint();
    }
    
    public void attachPickBehavior(MouseListener lis)
    {
        // System.out.println("attachPickBehavior = " + lis);
        if(MergeGraphicsOccuranceFactory.class.isInstance(lis))
        {
            ((DragableCanvas3D) canvas.getCanvas()).setupDragAndDrop();
        }
        else
        {
            canvas.getCanvas().addMouseListener(lis);
        }
    }
    
    public void removePickBehavior(MouseListener lis)
    {
        canvas.getCanvas().removeMouseListener(lis);
    }
    
    public void attachPickBehavior(final Canvas3D canvas3D)
    {
        
        // System.out.println("attachPickBehavior");
        canvas3D.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent me)
            {
                // System.out.println("in mouse clicked");
                
                final PickCanvas pickCanvas = new PickCanvas
										(canvas3D, canvas.getUniverse().getLocale());
                pickCanvas.setMode(PickTool.GEOMETRY_INTERSECT_INFO);
                pickCanvas.setTolerance(5.0f);
                pickCanvas.setShapeLocation(me);
                
                PickResult pickResult = pickCanvas.pickClosest();
                
                if (pickResult != null)
                {
                    
                    PickIntersection pickIntersection = 
												pickResult.getClosestIntersection
												(pickCanvas.getStartPosition());
                    
                    if (pickIntersection != null)
                    {
                        Point3d p3d1 = 
														pickIntersection.getClosestVertexCoordinates();
                        Point3d[] coords = 
														pickIntersection.getPrimitiveCoordinates();
                        int index = pickIntersection.getClosestVertexIndex();
                        int[] arrayIndex = 
														pickIntersection.getPrimitiveCoordinateIndices();
                        GeometryArray geometry = 
														pickIntersection.getGeometryArray();
                        
                        Point3d p3d2 = new Point3d();
                        geometry.getCoordinate(index, p3d2);
                        
                        
                        // System.out.print("Match between");
												/*
                        for(int i = 0; i < coords.length; ++i)
                        {
                            System.err.print(" point " + i + " = " + coords[i]);
                        }
                        System.err.println("");
												*/
                    }
                } else
                {
                    System.err.println("Cannot find line, please try again.");
                }
            }
        });
    }
    
    public BufferedImage getImage_old()
    {
        BufferedImage mainCanvasImage = null;
        BufferedImage xAxisMarksImage = null;
        BufferedImage yAxisMarksImage = null;
        BufferedImage xAxisTitleImage = null;
        // BufferedImage xSequenceImage = null;
        BufferedImage yAxisTitleImage = null;
        // BufferedImage ySequenceImage = null;
        BufferedImage mainTitleImage = null;
        
        mainCanvasImage = 
						ImageWriter.createBufferedImageFromCanvas3D(canvas.getCanvas());
        yAxisMarksImage = 
						ImageWriter.createBufferedImageFromComponent
						((Component) yAxisMarks);
        yAxisTitleImage = 
						ImageWriter.createBufferedImageFromComponent((Component) yTitle);
        mainTitleImage = 
						ImageWriter.createBufferedImageFromComponent((Component) mainTitle);
        
        BufferedImage wholeImage;
        
        if(yTitle.getHeight() != 0 && yTitle.getWidth() != 0)
        {
            yAxisTitleImage = 
								ImageWriter.createBufferedImageFromComponent
								((Component) yTitle);
            
            if(this.isAncestorOf(xAxisMarks))
            {
                xAxisMarksImage = 
										ImageWriter.createBufferedImageFromComponent
										((Component) xAxisMarks);
                xAxisTitleImage = 
										ImageWriter.createBufferedImageFromComponent
										((Component) xTitle);
                BufferedImage[] images =
                { mainTitleImage, yAxisMarksImage, yAxisTitleImage, 
									mainCanvasImage, xAxisMarksImage, xAxisTitleImage };
                int[] xCoords =
                { 2, 0, 1, 2, 2, 2, 2 };
                int[] yCoords =
                { 0, 1, 1, 1, 2, 3, 4 };
                wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
            }
            else
            {
                BufferedImage[] images =
                { yAxisMarksImage, mainCanvasImage };
                int[] xCoords =
                { 0, 1 };
                int[] yCoords =
                { 0, 0 };
                wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
            }
        }
        else
        {
            if(this.isAncestorOf(xAxisMarks))
            {
                xAxisMarksImage = 
										ImageWriter.createBufferedImageFromComponent
										((Component) xAxisMarks);
                xAxisTitleImage = ImageWriter.createBufferedImageFromComponent
										((Component) xTitle);
                BufferedImage[] images =
                { mainTitleImage, yAxisMarksImage, mainCanvasImage,
                  xAxisMarksImage, xAxisTitleImage };
                  int[] xCoords =
                  { 2, 0, 2, 2, 2, 2 };
                  int[] yCoords =
                  { 0, 1, 1, 2, 3, 4 };
                  wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
            }
            else
            {
                BufferedImage[] images =
                { yAxisMarksImage, mainCanvasImage };
                int[] xCoords =
                { 0, 1 };
                int[] yCoords =
                { 0, 0 };
                wholeImage = ImageWriter.joinImages(images, xCoords, yCoords);
            }
        }
        return wholeImage;
    }
    
    public BufferedImage getImage_old2()
    {
        ImageJoiner joiner = new ImageJoiner();
        // arguments are component, parent, x, y, xWeight, yWeight, width,
				// height, anchor, fill
        joiner.addComponent(mainTitle, this, 0, 0, 1, 1, 
														GridBagConstraints.REMAINDER, 1,
        GridBagConstraints.CENTER, GridBagConstraints.NONE);
        joiner.addComponent(yTitle, this, 0, 1, 1, 1, 1, 1,
        GridBagConstraints.CENTER, GridBagConstraints.NONE);
        joiner.addComponent(yAxisMarks, this, 1, 1, 1, 1, 1, 1,
        GridBagConstraints.CENTER, GridBagConstraints.NONE);
        joiner.addCanvas3D(canvas.getCanvas(), this, 2, 1, 1, 1, 1, 1,
        GridBagConstraints.CENTER, GridBagConstraints.NONE);
        joiner.addComponent(xAxisMarks, this, 2, 2, 1, 1, 1, 1,
        GridBagConstraints.CENTER, GridBagConstraints.NONE);
        joiner.addComponent(xTitle, this, 2, 3, 1, 1, 1, 1,
        GridBagConstraints.CENTER, GridBagConstraints.NONE);
        return joiner.joinImage();
    }
    
    /** gets the image of this component */    
    public BufferedImage getImage()
    {
        BufferedImage image = ImageWriter.createBufferedImageFromComponent
						(this);
        BufferedImage canvasImage = 
						ImageWriter.createBufferedImageFromCanvas3D(canvas.getCanvas());
        Graphics2D graphics = (Graphics2D) image.getGraphics();
        graphics.drawImage(canvasImage, null, canvas.getLocation().x, 
													 canvas.getLocation().y);
        
        Rectangle centreBounds = controls.getCentrePanel().getBounds();
        Rectangle bottomBounds = controls.getBottomPanel().getBounds();
        
        int x = centreBounds.x + centreBounds.width;
        int y = 0;
        int w = image.getWidth() - centreBounds.width;
        int h = image.getHeight() - bottomBounds.height;
        
        BufferedImage returnImage = image.getSubimage(x, y, w, h);
        
        return returnImage;
    }
    
    public BufferedImage getImage(boolean excludeControls)
    {
        BufferedImage image = 
						ImageWriter.createBufferedImageFromComponent(this);
        BufferedImage canvasImage = 
						ImageWriter.createBufferedImageFromCanvas3D(canvas.getCanvas());
        Graphics2D graphics = (Graphics2D) image.getGraphics();
        graphics.drawImage(canvasImage, null, canvas.getLocation().x, 
													 canvas.getLocation().y);
        
        BufferedImage returnImage;
        
        if(excludeControls)
        {
            Rectangle centreBounds = controls.getCentrePanel().getBounds();
            Rectangle bottomBounds = controls.getBottomPanel().getBounds();
            
            int x = centreBounds.x + centreBounds.width;
            int y = 0;
            int w = image.getWidth() - centreBounds.width;
            int h = image.getHeight() - bottomBounds.height;
            
            returnImage = image.getSubimage(x, y, w, h);
        }
        else
        {
            returnImage = image;
        }
        
        return returnImage;
    }
    
    /** adds the sequence position marker */    
    public Shape3D addSinglePositionMarker(double sequencePosition)
    {
        double maxY = viewPositionModel.getMaxY();
        double minY = viewPositionModel.getMinY();
        
        // canvas.addLine(sequencePosition, minY, sequencePosition, maxY);
        
        return canvas.addLine(sequencePosition, minY, sequencePosition, maxY);
    }
    
    /** adds the sequence position marker */    
    public Shape3D addYPositionMarker(double sequencePosition)
    {
        double maxX = viewPositionModel.getMaxX();
        double minX = viewPositionModel.getMinX();
        
        // canvas.addLine(sequencePosition, minY, sequencePosition, maxY);
        
        return canvas.addLine(minX, sequencePosition, maxX, sequencePosition);
    }
    
    /** removes the sequence position marker */    
    public void removeLine(Shape3D line)
    {
        canvas.removeLine(line);
    }
    
    public void parserChanged(ParserEvent e)
    {
        setMetadata((Metadata) 
										((Parser) e.getSource()).getMetadatas().elementAt
										(graphicNumber));
        originalGraphic.setSceneRoot((BranchGroup)(e.getGraphics()).
        elementAt(graphicNumber));
        // System.out.println(" = " + );
        ((TransferableGraphic) scenes.elementAt(0)).
        setSceneRoot((BranchGroup)(e.getGraphics()).
        elementAt(graphicNumber));
        ((TransferableGraphic) scenes.elementAt(0)).
        setMetadata((Metadata)(e.getMetadatas()).
        elementAt(graphicNumber));
    }
    
    public void viewPositionModelChanged(ViewPositionModelEvent e)
    {
        if(e.getPropertyName().equals("parser"))
        {
            originalGraphic.setModel((ViewPositionModel) e.getSource());
            ((TransferableGraphic) scenes.elementAt(0)).
            setModel((ViewPositionModel) e.getSource());
        }
    }
    public void groutPanelChanged(GroutPanelEvent e)
    {
        GroutPanel source = (GroutPanel) e.getSource();
        if(e.getPropertyName().equals("xAxisChange"))
        {
            if(((Boolean) e.getNewValue()).booleanValue())
            {
                removeXAxis();
                listenToThisControl(source.getControls());
            } else
            {
                addXAxis();
                listenToOwnControl();
            }
        }
        if(e.getPropertyName().startsWith("Font"))
        {
            String whatFont = e.getPropertyName();
            if(whatFont.substring(4).startsWith("XTitle"))
            {
                if(whatFont.substring(10).startsWith("Size"))
                {
                    int size = yTitleFont.getSize();
                    try
                    {
                        String sizeString = whatFont.substring(14);
                        size = Integer.parseInt(sizeString);
                        // System.out.println("size = " + size);
                    }
                    catch(NumberFormatException exc)
                    {
                        // this really should never happen
                        exc.printStackTrace();
                    }
                    xTitleFont = new Font(xTitleFont.getName(),
                    xTitleFont.getStyle(), size);
                    xTitle.setFont(xTitleFont);
                }
                if(whatFont.substring(10).startsWith("Colour"))
                {
                    if(whatFont.substring(16).startsWith("Pink"))
                    {
                        xTitle.setForeground(Color.pink);
                    }
                    if(whatFont.substring(16).startsWith("Red"))
                    {
                        xTitle.setForeground(Color.red);
                    }
                    if(whatFont.substring(16).startsWith("White"))
                    {
                        xTitle.setForeground(Color.white);
                    }
                    if(whatFont.substring(16).startsWith("Black"))
                    {
                        xTitle.setForeground(Color.black);
                    }
                }
                if(whatFont.substring(10).startsWith("SetText"))
                {
                    String newText = (String)JOptionPane.showInputDialog
                    (this,
                    "Enter text for X Title",
                    "Set Text",
                    JOptionPane.PLAIN_MESSAGE,
                    null,
                    null,
                    xTitle.getText());
                    if(newText != null)
                    {
                        xTitle.setText(newText);
                    }
                }
            } else if(whatFont.substring(4).startsWith("YTitle"))
            {
                if(whatFont.substring(10).startsWith("Size"))
                {
                    int size = yTitleFont.getSize();
                    try
                    {
                        String sizeString = whatFont.substring(14);
                        size = Integer.parseInt(sizeString);
                        // System.out.println("size = " + size);
                    }
                    catch(NumberFormatException exc)
                    {
                        // this really should never happen
                        exc.printStackTrace();
                    }
                    yTitleFont = new Font(yTitleFont.getName(),
                    yTitleFont.getStyle(), size);
                    yTitle.setFont(yTitleFont);
                }
                if(whatFont.substring(10).startsWith("Colour"))
                {
                    if(whatFont.substring(16).startsWith("Pink"))
                    {
                        yTitle.setForeground(Color.pink);
                    }
                    if(whatFont.substring(16).startsWith("Red"))
                    {
                        yTitle.setForeground(Color.red);
                    }
                    if(whatFont.substring(16).startsWith("White"))
                    {
                        yTitle.setForeground(Color.white);
                    }
                    if(whatFont.substring(16).startsWith("Black"))
                    {
                        yTitle.setForeground(Color.black);
                    }
                }
                if(whatFont.substring(10).startsWith("SetText"))
                {
                    String newText = (String)JOptionPane.showInputDialog
                    (this,
                    "Enter text for Y Title",
                    "Set Text",
                    JOptionPane.PLAIN_MESSAGE,
                    null,
                    null,
                    yTitle.getText());
                    if(newText != null)
                    {
                        yTitle.setText(newText);
                    }
                }
            } else if(whatFont.substring(4).startsWith("MainTitle"))
            {
                if(whatFont.substring(13).startsWith("Size"))
                {
                    int size = mainTitleFont.getSize();
                    try
                    {
                        String sizeString = whatFont.substring(17);
                        size = Integer.parseInt(sizeString);
                    }
                    catch(NumberFormatException exc)
                    {
                        // this really should never happen
                        exc.printStackTrace();
                    }
                    mainTitleFont = new Font(mainTitleFont.getName(),
                    mainTitleFont.getStyle(), size);
                    mainTitle.setFont(mainTitleFont);
                }
                if(whatFont.substring(13).startsWith("Colour"))
                {
                    if(whatFont.substring(19).startsWith("Pink"))
                    {
                        mainTitle.setForeground(Color.pink);
                    }
                    if(whatFont.substring(19).startsWith("Red"))
                    {
                        mainTitle.setForeground(Color.red);
                    }
                    if(whatFont.substring(19).startsWith("White"))
                    {
                        mainTitle.setForeground(Color.white);
                    }
                    if(whatFont.substring(19).startsWith("Black"))
                    {
                        mainTitle.setForeground(Color.black);
                    }
                }
                if(whatFont.substring(13).startsWith("SetText"))
                {
                    String newText = (String)JOptionPane.showInputDialog
                    (this,
                    "Enter text for Main Title",
                    "Set Text",
                    JOptionPane.PLAIN_MESSAGE,
                    null,
                    null,
                    mainTitle.getText());
                    if(newText != null)
                    {
                        mainTitle.setText(newText);
                    }
                }
            }
        }
    }
    
    /** This function is used a lot more than its name sugests
		 * a lot of the functions that change the apperance of the text call this
		 */
    public void changeFont(String whatFont, Color colour)
    {
        System.out.println("whatFont in changeFont = " + whatFont);
        if(whatFont.substring(4).startsWith("XTitle"))
        {
            if(whatFont.substring(10).startsWith("Size"))
            {
                int size = xTitleFont.getSize();
                try
                {
                    String sizeString = whatFont.substring(14);
                    size = Integer.parseInt(sizeString);
                    // System.out.println("size = " + size);
                }
                catch(NumberFormatException exc)
                {
                    // this really should never happen
                    exc.printStackTrace();
                }
                xTitleFont = new Font(xTitleFont.getName(),
                xTitleFont.getStyle(), size);
                xTitle.setFont(xTitleFont);
            }
            if(whatFont.substring(10).startsWith("Colour"))
            {
                if(whatFont.substring(16).startsWith("User"))
                {
                    xTitle.setForeground(colour);
                }
                if(whatFont.substring(16).startsWith("Pink"))
                {
                    xTitle.setForeground(Color.pink);
                }
                if(whatFont.substring(16).startsWith("Red"))
                {
                    xTitle.setForeground(Color.red);
                }
                if(whatFont.substring(16).startsWith("White"))
                {
                    xTitle.setForeground(Color.white);
                }
                if(whatFont.substring(16).startsWith("Black"))
                {
                    xTitle.setForeground(Color.black);
                }
            }
            if(whatFont.substring(10).startsWith("SetText"))
            {
                String newText = (String)JOptionPane.showInputDialog
                (this,
                "Enter text for X Title",
                "Set Text",
                JOptionPane.PLAIN_MESSAGE,
                null,
                null,
                xTitle.getText());
                // System.out.println("newText = " + newText);
                if(newText != null)
                {
                    xTitle.setText(newText);
                }
            }
        } else if(whatFont.substring(4).startsWith("YTitle"))
        {
            if(whatFont.substring(10).startsWith("Size"))
            {
                int size = yTitleFont.getSize();
                try
                {
                    String sizeString = whatFont.substring(14);
                    size = Integer.parseInt(sizeString);
                    // System.out.println("size = " + size);
                }
                catch(NumberFormatException exc)
                {
                    // this really should never happen
                    exc.printStackTrace();
                }
                yTitleFont = new Font(yTitleFont.getName(),
                yTitleFont.getStyle(), size);
                yTitle.setFont(yTitleFont);
            }
            if(whatFont.substring(10).startsWith("Colour"))
            {
                if(whatFont.substring(16).startsWith("User"))
                {
                    yTitle.setForeground(colour);
                }
                if(whatFont.substring(16).startsWith("Pink"))
                {
                    yTitle.setForeground(Color.pink);
                }
                if(whatFont.substring(16).startsWith("Red"))
                {
                    yTitle.setForeground(Color.red);
                }
                if(whatFont.substring(16).startsWith("White"))
                {
                    yTitle.setForeground(Color.white);
                }
                if(whatFont.substring(16).startsWith("Black"))
                {
                    yTitle.setForeground(Color.black);
                }
            }
            if(whatFont.substring(10).startsWith("SetText"))
            {
                String newText = (String)JOptionPane.showInputDialog
                (this,
                "Enter text for Y Title",
                "Set Text",
                JOptionPane.PLAIN_MESSAGE,
                null,
                null,
                yTitle.getText());
                yTitle.setText(newText);
                if(newText != null)
                {
                    yTitle.setText(newText);
                }
            }
        } else if(whatFont.substring(4).startsWith("MainTitle"))
        {
            if(whatFont.substring(13).startsWith("Size"))
            {
                int size = mainTitleFont.getSize();
                try
                {
                    String sizeString = whatFont.substring(17);
                    size = Integer.parseInt(sizeString);
                }
                catch(NumberFormatException exc)
                {
                    // this really should never happen
                    exc.printStackTrace();
                }
                mainTitleFont = new Font(mainTitleFont.getName(),
                mainTitleFont.getStyle(), size);
                mainTitle.setFont(mainTitleFont);
            }
            if(whatFont.substring(13).startsWith("Colour"))
            {
                if(whatFont.substring(19).startsWith("User"))
                {
                    mainTitle.setForeground(colour);
                }
                if(whatFont.substring(19).startsWith("Pink"))
                {
                    mainTitle.setForeground(Color.pink);
                }
                if(whatFont.substring(19).startsWith("Red"))
                {
                    mainTitle.setForeground(Color.red);
                }
                if(whatFont.substring(19).startsWith("White"))
                {
                    mainTitle.setForeground(Color.white);
                }
                if(whatFont.substring(19).startsWith("Black"))
                {
                    mainTitle.setForeground(Color.black);
                }
            }
            if(whatFont.substring(13).startsWith("SetText"))
            {
                String newText = (String)JOptionPane.showInputDialog
                (this,
                "Enter text for Main Title",
                "Set Text",
                JOptionPane.PLAIN_MESSAGE,
                null,
                null,
                mainTitle.getText());
                if(newText != null)
                {
                    mainTitle.setText(newText);
                }
            }
        }
        if(whatFont.substring(4).startsWith("AxisMarks"))
        {
            AxisMarks useTheseXAxisMarks;
            if(isAncestorOf(xAxisMarks))
            {
                useTheseXAxisMarks = xAxisMarks;
            }
            else
            {
                useTheseXAxisMarks = parentPanel.getXAxisMarks();
            }
            
            (new IntervalMenu(useTheseXAxisMarks, yAxisMarks)).show();
        }
        notifyTargets(whatFont, null, null);
    }
    
    public void writeObject(java.io.ObjectOutputStream out)
    throws IOException
    {
        // (new Exception()).printStackTrace();
        out.defaultWriteObject();
    }
    
    public void readObject(java.io.ObjectInputStream in)
    throws IOException, ClassNotFoundException
    {
        // (new Exception()).printStackTrace();
        in.defaultReadObject();
    }
    
    public String toString()
    {
        String returnString = super.toString() + " " + graphicNumber;
        return returnString;
        
    }
    
    public void graphicColourChanged(TransferableGraphic tr, Color colourSwing)
    {
        
        OneGraphic graphic = tr.getOriginalGraphic();
        int graphicNumber = graphic.getGraphicNumber();
        notifyTargets(("graphicColourChanged" + graphicNumber), null,
											colourSwing);
    }
    
    public synchronized void addOneGraphicListener
    (OneGraphicListener listener)
    {
        targets.addElement(listener);
    }
    
    public synchronized void removeOneGraphicListener
    (OneGraphicListener listener)
    {
        targets.removeElement(listener);
    }
    
    protected void notifyTargets(String propertyName, Object oldValue,
    Object newValue)
    {
        System.out.println("notifyTargets to " + propertyName);
        Vector l;
        OneGraphicEvent s =
        new OneGraphicEvent((Object) this, propertyName,
        oldValue, newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            OneGraphicListener sl =
            (OneGraphicListener) l.elementAt(i);
            //			System.out.println("viewPositionModelChanged " + sl);
            sl.oneGraphicChanged(s);
        }
    }
    
}

/** Object to add all the components together into one image */
class ImageJoiner
{
    Vector images = new Vector();
    Vector xCoords = new Vector();
    Vector yCoords = new Vector();
    Vector xWeights = new Vector();
    Vector yWeights = new Vector();
    Vector widths = new Vector();
    Vector heights = new Vector();
    Vector anchors = new Vector();
    Vector fills = new Vector();
    
    public void addComponent(Component component, JPanel parent, int xCoord,
    int yCoord, double xWeight, double yWeight,
    int width, int height, int anchor, int fill)
    {
        if((parent.isAncestorOf(component))
        && (component.getHeight() != 0 && component.getWidth() != 0))
        {
            images.addElement
            (ImageWriter.createBufferedImageFromComponent
						 ((Component) component));
            xCoords.addElement(new Integer(xCoord));
            yCoords.addElement(new Integer(yCoord));
            xWeights.addElement(new Double(xWeight));
            yWeights.addElement(new Double(yWeight));
            widths.addElement(new Integer(width));
            heights.addElement(new Integer(height));
            anchors.addElement(new Integer(anchor));
            fills.addElement(new Integer(fill));
        }
    }
    
    public void addCanvas3D(Canvas3D component, JPanel parent, int xCoord,
    int yCoord, double xWeight, double yWeight,
    int width, int height, int anchor, int fill)
    {
        if((parent.isAncestorOf(component))
        && (component.getHeight() != 0 && component.getWidth() != 0))
        {
            images.addElement
            (ImageWriter.createBufferedImageFromCanvas3D(component));
            xCoords.addElement(new Integer(xCoord));
            yCoords.addElement(new Integer(yCoord));
            xWeights.addElement(new Double(xWeight));
            yWeights.addElement(new Double(yWeight));
            widths.addElement(new Integer(width));
            heights.addElement(new Integer(height));
            anchors.addElement(new Integer(anchor));
            fills.addElement(new Integer(fill));
        }
    }
    
    public BufferedImage joinImage()
    {
        BufferedImage[] imageArray = vectorToImageArray(images);
        int[] xCoordsArray = vectorToIntArray(xCoords);
        int[] yCoordsArray = vectorToIntArray(yCoords);
        double[] xWeightsArray = vectorToDoubleArray(xWeights);
        double[] yWeightsArray = vectorToDoubleArray(yWeights);
        int[] widthsArray = vectorToIntArray(widths);
        int[] heightsArray = vectorToIntArray(heights);
        int[] anchorsArray = vectorToIntArray(anchors);
        int[] fillsArray = vectorToIntArray(fills);
        
        // System.out.println("BufferedImage joinImage");
        
        return ImageWriter.joinImagesGirdBag(imageArray, xCoordsArray,
        yCoordsArray, xWeightsArray,
        yWeightsArray, widthsArray,
        heightsArray, anchorsArray,
        fillsArray, null, null, null);
    }
    
    private int[] vectorToIntArray(Vector vector)
    {
        int[] returnArray = new int[vector.size()];
        for(int i = 0; i < vector.size(); ++i)
        {
            returnArray[i] = ((Integer) vector.elementAt(i)).intValue();
        }
        return returnArray;
    }
    
    private double[] vectorToDoubleArray(Vector vector)
    {
        double[] returnArray = new double[vector.size()];
        for(int i = 0; i < vector.size(); ++i)
        {
            returnArray[i] = ((Double) vector.elementAt(i)).doubleValue();
        }
        return returnArray;
    }
    
    private BufferedImage[] vectorToImageArray(Vector vector)
    {
        BufferedImage[] returnArray = new BufferedImage[vector.size()];
        for(int i = 0; i < vector.size(); ++i)
        {
            returnArray[i] = (BufferedImage) vector.elementAt(i);
        }
        return returnArray;
    }
    
}
