/*
 * OneGraphicEvent.java
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
public class OneGraphicEvent extends PropertyChangeEvent
{
  
  private OneGraphic source;
  
  /** Creates a new instance of OneGraphicEvent */
  public OneGraphicEvent(Object source, String propertyName, Object oldValue, Object newValue)
  {
    super(source, propertyName, oldValue, newValue);
    this.source = (OneGraphic) source;
  }
  
}
