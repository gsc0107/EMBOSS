/****************************************************************
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*  Based on EmbreoResList
*
*  @author: Copyright (C) Tim Carver
*
***************************************************************/

package org.emboss.jemboss.programs;

import java.util.*;

import org.emboss.jemboss.soap.JembossSoapException;
import org.emboss.jemboss.soap.PrivateRequest;
import org.emboss.jemboss.JembossParams;

/**
*
* Used by ShowSavedResults to show (show_saved_results),
* delete (delete_saved_results) & list results (list_saved_results).
*/
public class ResultList 
{
  private String statusmsg;
  private String status;
  private Hashtable proganswer;

/**
*
* Holds the list of stored results
* @param mysettings JembossParams defining server parameters
* @throws JembossSoapException If authentication fails
*
*/
  public ResultList(JembossParams mysettings) throws JembossSoapException 
  {
    this(mysettings,null,"list_saved_results");
  }

   public ResultList(JembossParams mysettings, String dataset,
                     String methodname) throws JembossSoapException
   {
     this(mysettings,dataset,"",methodname);
   }

/**
*
* Manipulate a dataset.
* @param mysettings JembossParams defining server parameters
* @param dataset    Which dataset to manipulate
* @param options    A project file to retrieve
* @param methodname What method to invoke on this dataset
*
*/
   public ResultList(JembossParams mysettings, String dataset, 
                     String options, String methodname) throws JembossSoapException 
   {
     PrivateRequest eRun;
     Vector params = new Vector();

     if(dataset != null) 
     {
       params.addElement(dataset);
       params.addElement(options);
     }

     try 
     {
       eRun = new PrivateRequest(mysettings,methodname, params);
     } 
     catch (JembossSoapException e) 
     {
       throw new JembossSoapException("Authentication Failed");
     }

     proganswer = eRun.getHash();
     status = proganswer.get("status").toString();
     statusmsg = proganswer.get("msg").toString();
     
     proganswer.remove("status");      //delete out of the hash
     proganswer.remove("msg");
   }

/**
*
* The status of the request
* @return String 0 for success, anything else for failure
*
*/
  public String getStatus() 
  {
    return status;
  }

/**
*
* A status message
* @return A string containing a status message. In the case of an error,
* contains a description of the error.
*
*/
  public String getStatusMsg() 
  {
    return statusmsg;
  }
  
/**
*
* @return Hashtable of the results
*
*/
  public Hashtable hash() 
  {
    return proganswer;
  }

/**
*
* @return Enumeration of the elements of the results Hashtable
*
*/
  public Enumeration elements() 
  {
    return proganswer.elements();
  }

/**
*
* @return Enumeration of the keys of the results Hashtable
*
*/
  public Enumeration keys() 
  {
    return proganswer.keys();
  }


/**
*
* @param Object key of the element to return
* @return the element in the result Hashtable defined by the key
*
*/
  public Object get(Object key) 
  {
    return proganswer.get(key);
  }

/**
*
* Replace the current results hash
* @param newres  The new results hash
*
*/
  public void updateRes(Hashtable newres)
  {
    proganswer = newres;
  }

}

