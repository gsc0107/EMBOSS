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
 * ControlContainerEvent.java
 *
 * Created on June 24, 2002, 11:46 AM
 */

package org.emboss.grout;

import java.util.EventObject;
import java.beans.PropertyChangeEvent;

/**
 *
 * @author  hmorgan2
 */
public class ControlContainerEvent extends PropertyChangeEvent
{
  
  private ControlContainer source;
  
  /** Creates a new instance of ControlContainerEvent */
  public ControlContainerEvent(Object source, String propertyName, Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (ControlContainer) source;
    
  }
  
}
