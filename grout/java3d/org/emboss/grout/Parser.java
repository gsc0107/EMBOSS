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
 * Parser.java
 *
 * Created on April 17, 2002, 3:31 PM
 */

package org.emboss.grout;

// my latest imports

import javax.media.j3d.SharedGroup;
import javax.media.j3d.Link;
//import ;

import java.beans.*;
import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BoundingBox;
import javax.media.j3d.Bounds;
import javax.vecmath.Point3d;
import java.io.StringReader;
import java.util.Enumeration;

import java.io.PrintWriter;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.BufferedReader;
import java.io.FileReader;

import javax.media.j3d.Node;
import javax.media.j3d.Group;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Shape3D;
import javax.media.j3d.Geometry;
import javax.media.j3d.GeometryArray;
import javax.media.j3d.Appearance;
import javax.media.j3d.Material;
import javax.media.j3d.IndexedGeometryArray;
import java.util.Vector;
import org.web3d.j3d.loaders.VRML97Loader;
import org.web3d.j3d.loaders.X3DLoader;
import java.io.File;
import java.net.URL;
import java.net.MalformedURLException;
import com.sun.j3d.loaders.Scene;
import java.net.ContentHandlerFactory;
import java.util.EventListener;
/** This handles parseing the x3d file
 * @author hmorgan2
 */
public class Parser extends java.lang.Object implements java.io.Serializable
{
    
    public BranchGroup sceneRoot = null;
    /** this holds all the scene roots */    
    private Vector allGraphics = null;
    /** this holds the metadat for all the graphics */    
    private Vector metadatas = null;
    private Metadata groutOptions = new Metadata();
    private Scene scene = null;
    private String filename = null;
    private BoundingBox wholeBoundsBox = null;
    private BoundingSphere wholeBoundsSphere = null;
    
    private PropertyChangeSupport propertySupport;
    private Vector targets = new Vector();
    
    private	GroutPanel ownerPanel;
    private	String file;
    
    //	private BoundingBox bounds = null;;
    
    /** Creates a new instance of Parser */
    public Parser()
    {
        allGraphics = new Vector();
        filename = "/people/hmorgan2/code/samples/ajb2.wrl";
        propertySupport = new PropertyChangeSupport( this );
    }
    
    protected void notifyTargets()
    {
        Vector l;
        ParserEvent s = new ParserEvent((Object) this, "Filename",
																				"oldFileName", filename);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            
            ParserListener sl = (ParserListener) l.elementAt(i);
            sl.parserChanged(s);
        }
    }
    
    protected void notifyTargets(String propertyName, Object oldValue, 
																 Object newValue)
    {
        Vector l;
        ParserEvent s = new ParserEvent((Object) this, propertyName, oldValue,
																				newValue);
        synchronized(this)
        {
            l = (Vector) targets.clone();
        }
        for (int i = 0; i < l.size(); i++)
        {
            ParserListener sl = (ParserListener) l.elementAt(i);
            sl.parserChanged(s);
        }
    }
    
    
    /** Loads a scene
     * @param fn filename
     */
    void loadScene(String fn)
    {
        filename = fn;
        loadScene();
    }
    
    /** Loads a scene from this string
     * @param string the string
     */    
    public void loadSceneFromString(String string)
    {
        file = string;

        X3DLoader vrmlLoader = new X3DLoader(X3DLoader.LOAD_ALL);
        // // System.out.println("string = " + string.substring(1, 200));
        StringReader sr = new StringReader(string);
        try
        {
						//this is so ugly.  How can I sort it out? hugh
            vrmlLoader.setBaseUrl(new URL("http://www.hgmp.mrc.ac.uk/"));
        } catch (java.net.MalformedURLException e)
        { e.printStackTrace();}
        scene = vrmlLoader.load(sr);
        if (sceneRoot != null)
        {
            sceneRoot.detach();
        }
        if (scene != null)
        {
            // get the scene group
            sceneRoot = scene.getSceneGroup();
            // Do we want these?
            sceneRoot.setCapability(BranchGroup.ALLOW_DETACH);
            sceneRoot.setCapability(BranchGroup.ALLOW_BOUNDS_READ);
            setAllCapabilitys((Group) sceneRoot);
            setAllCapabilitysOfAllGraphics();
            // Do we want to do this here? I think not
            // sceneRoot.compile();
        }
        metadatas = MetadataFactory.parseStringForMetadata(string, groutOptions);
        //		MetadataFactory.printAll(metadatas);
    }
    
    
    /** loads a scene from the set filename */    
    public void loadScene()
    {
        loadSceneFromString(loadFile(filename));
    }
    
    public void loadScene_old()
    {
        X3DLoader vrmlLoader = new X3DLoader(X3DLoader.LOAD_ALL);
        File f = new File(filename);
        if(f.exists() && f.isFile())
        {
            URL loadUrl = null;
            try
            {
                loadUrl = new URL(filename);
            } catch (MalformedURLException badUrl)
            {
                // if the location is not a URL, this is what you get
                // this is run if you specify a file
								// nothing, which is right
            }
            
            try
            {
                if(loadUrl != null)
                    scene = vrmlLoader.load(loadUrl);
                else
                    scene = vrmlLoader.load(filename);
            } catch (Exception e)
            {
                // // System.out.println("Exception loading URL:" + e);
                e.printStackTrace();
            }
        } else
        {
            // // System.out.println("No file specified");
        }
        if (sceneRoot != null)
        {
            sceneRoot.detach();
        }
        if (scene != null)
        {
            // get the scene group
            sceneRoot = scene.getSceneGroup();
            // Do we want these?
            sceneRoot.setCapability(BranchGroup.ALLOW_DETACH);
            sceneRoot.setCapability(BranchGroup.ALLOW_BOUNDS_READ);
            setAllCapabilitys((Group) sceneRoot);
            setAllCapabilitysOfAllGraphics();
        }
    }
    
    /** sets the capability biuts of all the graphics */    
    public void setAllCapabilitysOfAllGraphics()
    {
        for(int i = 0; i < allGraphics.size(); ++i)
        {
            setAllCapabilitys((Group) allGraphics.elementAt(i));
        }
    }
    
    
    /** used recusivly to set all the capability bits of the graphics */    
    public static void setAllCapabilitys(Group sceneRoot)
    {
        int i = 0;
        //System.out.println("sceneRoot = " + sceneRoot);
        sceneRoot.setCapability(Group.ALLOW_CHILDREN_READ);
        sceneRoot.setCapability(Group.ALLOW_CHILDREN_WRITE);
        sceneRoot.setCapability(Group.ALLOW_CHILDREN_EXTEND);
        // System.out.println("ALLOW_CHILDREN_READ for group  = " + sceneRoot);
        for (Enumeration children = sceneRoot.getAllChildren(); 
						 children.hasMoreElements() ; i++)
        {
            Node child = (Node) children.nextElement();
            try
            {
                SharedGroup thisGroup = ((Link) child).getSharedGroup();
                setAllCapabilitys(thisGroup);
                ((Link) child).setCapability(Link.ALLOW_SHARED_GROUP_READ);
            } catch (java.lang.ClassCastException excep)
            {
                try
                {
                    /* swop this line to read scene graph */
                    setAllCapabilitys((Group) child);
                    // setAllCapabilitys((Group) child, 1);
                    ((Group) child).setCapability(Group.ALLOW_CHILDREN_READ);
                } catch (java.lang.ClassCastException e)
                {
                    try
                    {
                        ((Shape3D) child).setCapability
														(Shape3D.ALLOW_GEOMETRY_READ);
                        ((Shape3D) child).setCapability
														(Shape3D.ALLOW_BOUNDS_READ);
                        ((Shape3D) child).setCapability
														(Shape3D.ALLOW_APPEARANCE_READ);
                        ((Shape3D) child).setCapability
														(Shape3D.ALLOW_APPEARANCE_WRITE);
                        
                        Appearance appearance = 
														((Shape3D) child).getAppearance();
                        if(appearance != null)
                        {
                            appearance.setCapability
																(Appearance.ALLOW_MATERIAL_READ);
                            appearance.setCapability
																(Appearance.ALLOW_MATERIAL_WRITE);
                            
                            Material material = appearance.getMaterial();
                            if(material != null)
                            {
                                material.setCapability
																		(Material.ALLOW_COMPONENT_READ);
                                material.setCapability
																		(Material.ALLOW_COMPONENT_WRITE);
                            }
                        }
                        
                        // System.out.println("setCapability of " + child);
                        for(int j = 0; 
														j < (((Shape3D) child).numGeometries()); ++j)
                        {
                            // System.out.println("child = " + child);
                            Geometry geometry = ((Shape3D) child).getGeometry(j);
                            if(geometry != null)
                            {
                                // System.out.println("geometry = " + geometry);
                                geometry.setCapability
																		(IndexedGeometryArray.ALLOW_REF_DATA_READ);
                                geometry.setCapability
																		(IndexedGeometryArray.ALLOW_REF_DATA_WRITE);
                                try
                                {
                                    IndexedGeometryArray array = 
																				((IndexedGeometryArray) geometry);
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COORDINATE_READ);
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COORDINATE_WRITE);
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COUNT_READ);
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COUNT_WRITE);
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_FORMAT_READ);
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COORDINATE_INDEX_READ);
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COLOR_INDEX_READ );
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COLOR_INDEX_WRITE );
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COLOR_READ );
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_COLOR_WRITE );
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_TEXCOORD_READ );
                                    array.setCapability
																				(IndexedGeometryArray.ALLOW_TEXCOORD_WRITE );
                                    //							array.setCapability(GeometryArray.);
                                } catch (java.lang.ClassCastException exc)
                                {
                                    try
                                    {
                                        GeometryArray array = ((GeometryArray) geometry);
                                        // System.out.println("GeometryArray = " + array);
                                        array.setCapability
																						(IndexedGeometryArray.ALLOW_COORDINATE_READ);
                                        array.setCapability
																						(IndexedGeometryArray.ALLOW_COUNT_READ);
                                        array.setCapability
																						(IndexedGeometryArray.ALLOW_FORMAT_READ);
                                        array.setCapability
																						(IndexedGeometryArray.ALLOW_COLOR_READ );
                                        array.setCapability
																						(IndexedGeometryArray.ALLOW_COLOR_WRITE );
                                        array.setCapability
																						(IndexedGeometryArray.ALLOW_TEXCOORD_READ );
                                        array.setCapability
																						(IndexedGeometryArray.ALLOW_TEXCOORD_WRITE );
                                    } catch (java.lang.ClassCastException excp)
                                    {
                                    }
                                }
                            }
                        }
                    } catch (java.lang.ClassCastException ex)
                    {
                    }
                }
            }
        }
    }
    
    /** used recusivly to set all the capability bits of the graphics, also prints out
     * the nodes found
     * Not sorted out formating, only used for debuging
     */    
    public static void setAllCapabilitys(Group sceneRoot, int level)
    {
        Class indexedGeometryArrayClass = null;
        Class indexedLineStripArrayClass = null;
        try
        {
            indexedGeometryArrayClass =
            Class.forName("javax.media.j3d.IndexedGeometryArray");
            indexedLineStripArrayClass =
            Class.forName("javax.media.j3d.IndexedLineStripArray");
        }
        catch(ClassNotFoundException e)
        {
            e.printStackTrace();
        }
        int i = 0;
        System.out.println("sceneRoot = " + sceneRoot + " level = " + level);
        sceneRoot.setCapability(Group.ALLOW_CHILDREN_READ);
        for (Enumeration children = sceneRoot.getAllChildren(); children.hasMoreElements() ; i++)
        {
            Node child = (Node) children.nextElement();
            try
            {
                SharedGroup thisGroup = ((Link) child).getSharedGroup();
                setAllCapabilitys(thisGroup, (level + 1));
                ((Link) child).setCapability(Link.ALLOW_SHARED_GROUP_READ);
                System.out.println("ALLOW_CHILDREN_READ for group  = " + child);
            } catch (java.lang.ClassCastException excep)
            {
                try
                {
                    setAllCapabilitys((Group) child, (level + 1));
                    ((Group) child).setCapability(Group.ALLOW_CHILDREN_READ);
                    System.out.println("ALLOW_CHILDREN_READ for group  = " + child);
                } catch (java.lang.ClassCastException e)
                {
                    try
                    {
                        System.out.println("numGeometries = " + 
																					 ((Shape3D) child).numGeometries());
                        ((Shape3D) child).setCapability(Shape3D.ALLOW_GEOMETRY_READ);
                        ((Shape3D) child).setCapability(Shape3D.ALLOW_BOUNDS_READ);
                        // System.out.println("setCapability of " + child);
                        for(int j = 0; j < (((Shape3D) child).numGeometries()); ++j)
                        {
                            Geometry geometry = ((Shape3D) child).getGeometry(j);
                            System.out.println("Shape3D = " + child + " geometry = " + geometry + " class " + geometry.getClass().getName());
                            // if(geometry.getClass().isAssignableFrom(indexedGeometryArrayClass))
                            System.out.println("geometry.getClass().isAssignableFrom(indexedGeometryArrayClass) " + 
geometry.getClass().isAssignableFrom(indexedGeometryArrayClass));
                            System.out.println("reverse geometry.getClass().isAssignableFrom(indexedGeometryArrayClass) " + indexedGeometryArrayClass.isAssignableFrom(geometry.getClass()));
                            System.out.println("indexedGeometryArrayClass = " + indexedGeometryArrayClass);
                            System.out.println("indexedLineStripArrayClass = " + indexedLineStripArrayClass);
                            System.out.println("geometry.getClass().isAssignableFrom(indexedLineStripArrayClass) " + geometry.getClass().isAssignableFrom(indexedLineStripArrayClass));
                            
                            System.out.println("geometry.getClass() = " + geometry.getClass());
                            // try
                            // if(indexedGeometryArrayClass.isAssignableFrom(geometry.getClass()))
                            if(geometry instanceof IndexedGeometryArray)
                            {
                                IndexedGeometryArray array = ((IndexedGeometryArray) geometry);
                                System.out.println("array = " + array + " geometry = " + geometry);
                                //																							System.out.println("getCoordinateIndex = " + array.getCoordinateIndex());
                                // System.out.println("getCoordinateIndices = " + array.getCoordinateIndices());
                                System.out.println("getIndexCount = " + array.getIndexCount());
                                System.out.println("getInitialIndexIndex = " + array.getInitialIndexIndex());
                                // System.out.println("getCoordinates = " + array.getCoordinates());
                                // System.out.println("getColors = " + array.getColors());
                                // System.out.println(" = " + array.);
                                for(int k = 0; k < array.getIndexCount(); ++k)
                                {
                                    Point3d coordinate = new Point3d();
                                    array.getCoordinate(k, coordinate);
                                    System.out.println("coordinate " + k + " = " + coordinate);
                                }
                                array.setCapability(IndexedGeometryArray.ALLOW_COORDINATE_READ);
                                array.setCapability(IndexedGeometryArray.ALLOW_COUNT_READ);
                                array.setCapability(IndexedGeometryArray.ALLOW_FORMAT_READ);
                                array.setCapability(IndexedGeometryArray.ALLOW_COORDINATE_INDEX_READ);
                                array.setCapability(IndexedGeometryArray.ALLOW_COLOR_INDEX_READ );
                                array.setCapability(IndexedGeometryArray.ALLOW_COLOR_INDEX_WRITE );
                                array.setCapability(IndexedGeometryArray.ALLOW_TEXCOORD_READ );
                                array.setCapability(IndexedGeometryArray.ALLOW_TEXCOORD_WRITE );
                                //							array.setCapability(GeometryArray.);
                            }
                                                                                        /*
                                                                                                catch (java.lang.ClassCastException exc)
                                                                                                {
                                                                                                // System.out.println("Could not set capabilitys of " + geometry);
                                                                                                //							e.printStackTrace();
                                                                                                }
                                                                                         */
                        }
                    } catch (java.lang.ClassCastException ex)
                    {
                        // System.out.println("Could not get children of " + child);
                        //					e.printStackTrace();
                    }
                }
            }
            
        }
    }
    
    public static BoundingBox calculateBoundingSphere(Group sceneRoot)
    {
        int i = 0;
        BoundingBox returnBounds = null;
        // System.out.println("sceneRoot = " + sceneRoot);
        for (Enumeration children = sceneRoot.getAllChildren();
						 children.hasMoreElements() ; i++)
        {
            Node child = (Node) children.nextElement();
            // System.out.println("child = " + child);
            try
            {
                SharedGroup thisGroup = ((Link) child).getSharedGroup();
                if(returnBounds == null)
                {
                    returnBounds = calculateBoundingSphere(thisGroup);
                } else
                {
                    returnBounds.combine(calculateBoundingSphere(thisGroup));
                }
            } catch (java.lang.ClassCastException excep)
            {
                try
                {
                    if(returnBounds == null)
                    {
                        returnBounds = calculateBoundingSphere((Group) child);
                    } else
                    {
                        returnBounds.combine(calculateBoundingSphere
																						 ((Group) child));
                    }
                } catch (java.lang.ClassCastException e)
                {
                    try
                    {
                        if(returnBounds == null)
                        {
                            returnBounds = (BoundingBox) 
																((Shape3D) child).getBounds();
                        } else
                        {
                            returnBounds.combine(((Shape3D) child).getBounds());
                        }
                    } catch (java.lang.ClassCastException ex)
                    {
                        // System.out.println("Could not get bounds of " 
												// 									 + child);
                    }
                }
            }
            
        }
        // System.out.println("returnBounds = " + returnBounds);
        return returnBounds;
    }
    
    public static BoundingSphere calculateBoundingSphereAnotherOld(BranchGroup 
																																	 objRoot)
    {
        BoundingSphere bs = (BoundingSphere)objRoot.getBounds();
        Point3d center = new Point3d();
        bs.getCenter(center);
        double radius = bs.getRadius();
        return bs;
    }
    
    public BoundingBox calculateBoundingSphere(int whichGraphic)
    {
        BranchGroup objRoot = (BranchGroup) allGraphics.elementAt(whichGraphic);
        BoundingBox bb = calculateBoundingSphere(objRoot);
        return bb;
    }
    
    public void calculateMaxMinValuesOld(int whichGraphic, int xMin, 
																				 int xMax, int yMin, int yMax)
    {
        BranchGroup objRoot = (BranchGroup) allGraphics.elementAt(whichGraphic);
        BoundingSphere bs = (BoundingSphere)objRoot.getBounds();
        Point3d centre = new Point3d();
        bs.getCenter(centre);
        double radius = bs.getRadius();
        double xStart = centre.x - (radius / Math.pow(2, 0.5));
        double xEnd = centre.x + (radius / Math.pow(2, 0.5));
        double yStart = centre.y - (radius / Math.pow(2, 0.5));
        double yEnd = centre.y + (radius / Math.pow(2, 0.5));
        
        
        xMin = (int) xStart;
        xMax = (int) xEnd;
        yMin = (int) yStart;
        yMax = (int) yEnd;
        // // System.out.println("xMax = " + xMax );
    }
    
    /** calculate the min max values of a specific graphic
     * @param whichGraphic which Graphic
     * @return The min max vales, in the the form -
     *
     * returnArray[0] = (double) xStart;
     * returnArray[1] = (double) xEnd;
     * returnArray[2] = (double) yStart;
     * returnArray[3] = (double) yEnd;
     * returnArray[4] = (double) radius;
     */    
    public double[] calculateMaxMinValues(int whichGraphic)
    {
        BranchGroup objRoot = (BranchGroup) allGraphics.elementAt(whichGraphic);
        // System.out.println("objRoot = " + objRoot);
        BoundingSphere bs = (BoundingSphere)objRoot.getBounds();
        // System.out.println("bs = " + bs);
        double radius = bs.getRadius();
        //		// System.out.println("bs = " + bs);
        //        Point3d centre = new Point3d();
        //        bs.getCenter(centre);
        
        BoundingBox bb = calculateBoundingSphere(objRoot);
        Point3d lowerPoint = new Point3d();
        // System.out.println("bb = " + bb + " lowerPoint = " + lowerPoint);
        bb.getLower(lowerPoint);
        Point3d upperPoint = new Point3d();
        bb.getUpper(upperPoint);
        double xStart = lowerPoint.x;
        double xEnd = upperPoint.x;
        double yStart = lowerPoint.y;
        double yEnd = upperPoint.y;
        
        double[] returnArray = new double[5];
        
        returnArray[0] = (double) xStart;
        returnArray[1] = (double) xEnd;
        returnArray[2] = (double) yStart;
        returnArray[3] = (double) yEnd;
        returnArray[4] = (double) radius;
        
        // // System.out.println("returnArray[1] = " + returnArray[1]);
        
        return returnArray;
    }
    
    /** calculate the min max values of all the graphics
     * @return The min max vales, in the the form -
     *
     * returnArray[0] = (double) xStart;
     * returnArray[1] = (double) xEnd;
     * returnArray[2] = (double) yStart;
     * returnArray[3] = (double) yEnd;
     * returnArray[4] = (double) radius;
     */    
    public double[] calculateMaxMinValues()
    {
        // System.out.println("wholeBoundsSphere = " + wholeBoundsSphere);
        double radius = wholeBoundsSphere.getRadius();
        //		// System.out.println("bs = " + bs);
        //        Point3d centre = new Point3d();
        //        bs.getCenter(centre);
        
        Point3d lowerPoint = new Point3d();
        // System.out.println("lowerPoint = " + lowerPoint);
        wholeBoundsBox.getLower(lowerPoint);
        Point3d upperPoint = new Point3d();
        wholeBoundsBox.getUpper(upperPoint);
        double xStart = lowerPoint.x;
        double xEnd = upperPoint.x;
        double yStart = lowerPoint.y;
        double yEnd = upperPoint.y;
        
        double[] returnArray = new double[5];
        
        returnArray[0] = (double) xStart;
        returnArray[1] = (double) xEnd;
        returnArray[2] = (double) yStart;
        returnArray[3] = (double) yEnd;
        returnArray[4] = (double) radius;
        
        // // System.out.println("returnArray[1] = " + returnArray[1]);
        
        return returnArray;
    }
    
    /** calculate the min max values of a specific graphic
     * @param whichGraphic
     * @return The min max vales, in the the form -
     *
     * returnArray[0] = (double) xStart;
     * returnArray[1] = (double) xEnd;
     * returnArray[2] = (double) yStart;
     * returnArray[3] = (double) yEnd;
     * returnArray[4] = (double) radius;
     */    
    public double[] calculateMaxMinValuesD(int whichGraphic)
    {
        BranchGroup objRoot = (BranchGroup) allGraphics.elementAt(whichGraphic);
        BoundingSphere bs = (BoundingSphere)objRoot.getBounds();
        Point3d centre = new Point3d();
        bs.getCenter(centre);
        double radius = bs.getRadius();
        double xStart = centre.x - (radius / Math.pow(2, 0.5));
        double xEnd = centre.x + (radius / Math.pow(2, 0.5));
        double yStart = centre.y - (radius / Math.pow(2, 0.5));
        double yEnd = centre.y + (radius / Math.pow(2, 0.5));
        
        
        double[] returnArray = new double[5];
        
        returnArray[0] =  xStart;
        returnArray[1] =  xEnd;
        returnArray[2] =  yStart;
        returnArray[3] =  yEnd;
        returnArray[4] =  radius;
        
        // System.out.println("returnArray[1] = " + returnArray[1]);
        
        return returnArray;
    }
    
    public String getFilename()
    {
        return filename;
    }
    
    public void setFilename(String value)
    {
        String oldValue = filename;
        filename = value;
        loadScene();
        splitGraphics();
        propertySupport.firePropertyChange("Filename", oldValue, filename);
        notifyTargets();
        fireGroutOptionEvents();
        // // System.out.println("allGraphics = " + allGraphics);
        // // System.out.println("filename = " + filename);
        // // System.out.println("sceneRoot = " + sceneRoot);
    }
    
    /** sets the x3d file from a string */    
    public void setMetaFile(String value)
    {
        String oldValue = filename;
        filename = "temp.out";
        // // System.out.println("value = " + value.substring(1, 200));
        loadSceneFromString(value);
        splitGraphics();
        propertySupport.firePropertyChange("Filename", oldValue, filename);
        notifyTargets();
        fireGroutOptionEvents();
        // // System.out.println("allGraphics = " + allGraphics);
        // // System.out.println("filename = " + filename);
        // // System.out.println("sceneRoot = " + sceneRoot);
    }
    
    public void setOwnerPanel(GroutPanel op)
    {
        ownerPanel = op;
        return;
    }
    
    /** returns the various Graphics groups as a vector */
    public Vector getGraphics()
    {
        return allGraphics;
    }
    
    /** returns the various Metadatas as a vector */
    public Vector getMetadatas()
    {
        return metadatas;
    }
    
    /** returns the various Graphics groups as a vector */
    public int getNumberOfGraphics()
    {
        return allGraphics.size();
    }
    
    /** splits the various Graphics groups up and puts them in a vector */
    public void splitGraphics()
    {
        wholeBoundsBox = calculateBoundingSphere(sceneRoot);
        wholeBoundsSphere = (BoundingSphere) sceneRoot.getBounds();
        // System.out.println("spliting graphics, sceneRoot = " + sceneRoot);
        //    allGraphics.addElement(sceneRoot);
        BranchGroup child = (BranchGroup) sceneRoot.getChild(1);
        // System.out.println("child = " + child);
        
        BranchGroup grandChild = (BranchGroup) child.getChild(0);
        for(Enumeration graphics = grandChild.getAllChildren(); 
						graphics.hasMoreElements() ;)
        {
            Node graphic = (Node) graphics.nextElement();
            allGraphics.addElement(graphic);
            graphic.setCapability(BranchGroup.ALLOW_DETACH);
            graphic.setCapability(BranchGroup.ALLOW_BOUNDS_READ);
            // System.out.println("Graphic split = " + graphic);
            // Use this line if you want to remove them
            grandChild.removeChild(graphic);
        }
        if(allGraphics.size() == 0)
        {
            allGraphics.addElement(sceneRoot);
        }
        
        ownerPanel.graphicsSplit();
    }
    
    /** conveniece method.  Loads the contece of a file into a string */    
    public static String loadFile(String filename)
    {
        StringBuffer inputReader = new StringBuffer();
        try
        {
            BufferedReader br = new BufferedReader(new FileReader(filename));
            String text;
            while ((text = br.readLine()) != null)
            {
                inputReader = inputReader.append(text).append("\n");
            }
            // Always close a file after opening.
            br.close();
        } catch(java.io.IOException e)
        {
            e.printStackTrace();
        }
        return inputReader.toString();
    }
    
    /** Removes all data from this parser, ready to load another file */    
    public void clearAllData()
    {
        allGraphics.removeAllElements();
        metadatas.removeAllElements();
    }
    
    /** sets up the grout panel from the options in the x3d file */    
    public void fireGroutOptionEvents()
    {
        // I could do it this way, may be cleaner to do it with events,
        // but I do not like it (see below)
        
        ownerPanel.setOptions(groutOptions);
        
    }
    
    public String getFile()
    {
        return file;
    }
    
    public void saveFile()
    {
        File outputFile = GroutFileChooser.showInputDialogFile
        (ownerPanel, "Save as", "image");
        try
        {
            FileWriter fw = new FileWriter(outputFile);
            fw.write(file);
            fw.close();
        }
        catch(java.io.IOException e)
        {
            e.printStackTrace();
        }
    }
    
    public synchronized void addParserListener(ParserListener listener)
    {
        targets.addElement(listener);
    }
    
    public synchronized void removeParserListener(ParserListener listener)
    {
        targets.removeElement(listener);
    }
    
    public void addPropertyChangeListener(PropertyChangeListener listener)
    {
        propertySupport.addPropertyChangeListener(listener);
    }
    
    public void removePropertyChangeListener(PropertyChangeListener listener)
    {
        propertySupport.removePropertyChangeListener(listener);
    }
    
    public static void main(String[] args)
    {
        Parser parser = new Parser();
        parser.loadScene(args[0]);
        //    parser.splitGraphics();
        // Vector graphics = parser.getGraphics();
        System.out.println(parser.sceneRoot);
        parser.setAllCapabilitys(parser.sceneRoot);
        System.exit(0);
    }
    
}
