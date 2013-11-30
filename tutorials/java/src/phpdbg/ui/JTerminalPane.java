/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package phpdbg.ui;

/**
 *
 * @author krakjoe
 */
import javax.swing.*;
import javax.swing.text.*;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.io.IOException;
import javax.imageio.ImageIO;

public class JTerminalPane extends JTextPane {
  
  private static final Color D_Black   = Color.getHSBColor( 0.000f, 0.000f, 0.000f );
  private static final Color D_Red     = Color.getHSBColor( 0.000f, 1.000f, 0.502f );
  private static final Color D_Blue    = Color.getHSBColor( 0.667f, 1.000f, 0.502f );
  private static final Color D_Magenta = Color.getHSBColor( 0.833f, 1.000f, 0.502f );
  private static final Color D_Green   = Color.getHSBColor( 0.333f, 1.000f, 0.502f );
  private static final Color D_Yellow  = Color.getHSBColor( 0.167f, 1.000f, 0.502f );
  private static final Color D_Cyan    = Color.getHSBColor( 0.500f, 1.000f, 0.502f );
  private static final Color D_White   = Color.getHSBColor( 0.000f, 0.000f, 0.753f );
  private static final Color B_Black   = Color.getHSBColor( 0.000f, 0.000f, 0.502f );
  private static final Color B_Red     = Color.getHSBColor( 0.000f, 1.000f, 1.000f );
  private static final Color B_Blue    = Color.getHSBColor( 0.667f, 1.000f, 1.000f );
  private static final Color B_Magenta = Color.getHSBColor( 0.833f, 1.000f, 1.000f );
  private static final Color B_Green   = Color.getHSBColor( 0.333f, 1.000f, 1.000f );
  private static final Color B_Yellow  = Color.getHSBColor( 0.167f, 1.000f, 1.000f );
  private static final Color B_Cyan    = Color.getHSBColor( 0.500f, 1.000f, 1.000f );
  private static final Color B_White   = Color.getHSBColor( 0.000f, 0.000f, 1.000f );
  private static final Color cReset    = Color.getHSBColor( 0.000f, 0.000f, 1.000f );

  private Color colorCurrent    = cReset;
  private String remaining = "";
  
  public JTerminalPane() {
      super();
      setOpaque(false);
      setBackground(new Color(0, 0, 0, 0));
  }
  
  @Override public void paintComponent(Graphics g) {
      g.setColor(Color.BLACK);
      g.fillRect(0, 0, getWidth(), getHeight());
      
      try {
          Image image = ImageIO.read(
                  JTerminalPane.class.getResource("logo-small.png"));
          
          g.drawImage(
                  image, 
                  getWidth() - image.getWidth(this) - 10, 
                  getHeight() - image.getHeight(this) - 10, 
                  image.getWidth(this), image.getHeight(this), this);
          
      } catch (IOException | NullPointerException | IllegalArgumentException ex) {}
      
      super.paintComponent(g);
  }
  
  public void append(Color c, String s) {
    StyleContext sc = StyleContext.getDefaultStyleContext();
    AttributeSet aset = sc.addAttribute(
            SimpleAttributeSet.EMPTY, StyleConstants.Foreground, c);
    setCharacterAttributes(aset, false);
    replaceSelection(s);
    setCaretPosition(getDocument().getLength());
  }

  public synchronized void appendANSI(String s) {
    int aPos = 0;
    int aIndex;
    int mIndex;
    String tmpString;
    boolean stillSearching;
    
    String addString = remaining + s;
    
    remaining = "";

    if (addString.length() > 0) {
      aIndex = addString.indexOf("\u001B");
      if (aIndex == -1) {
        append(colorCurrent,addString);
        return;
      }

      if (aIndex > 0) {
        tmpString = addString.substring(0,aIndex);
        append(colorCurrent, tmpString);
        aPos = aIndex;
      }

      stillSearching = true;
      while (stillSearching) {
        mIndex = addString.indexOf("m",aPos);
        if (mIndex < 0) {
          remaining = addString.substring(aPos,addString.length());
          stillSearching = false;
          continue;
        }
        else {
          tmpString = addString.substring(aPos,mIndex+1);
          colorCurrent = getANSIColor(tmpString);
        }
        aPos = mIndex + 1;

        aIndex = addString.indexOf("\u001B", aPos);

        if (aIndex == -1) {
          tmpString = addString.substring(aPos,addString.length());
          append(colorCurrent, tmpString);
          stillSearching = false;
          continue; 
        }
        tmpString = addString.substring(aPos,aIndex);
        aPos = aIndex;
        append(colorCurrent, tmpString);
      }
    }
  }

  public Color getANSIColor(String ANSIColor) {
      switch (ANSIColor) {
          case "\u001B[30m":
              return D_Black;
          case "\u001B[31m":
              return D_Red;
          case "\u001B[32m":
              return D_Green;
          case "\u001B[33m":
              return D_Yellow;
          case "\u001B[34m":
              return D_Blue;
          case "\u001B[35m":
              return D_Magenta;
          case "\u001B[36m":
              return D_Cyan;
          case "\u001B[37m":
              return D_White;
          case "\u001B[0;30m":
              return D_Black;
          case "\u001B[0;31m":
              return D_Red;
          case "\u001B[0;32m":
              return D_Green;
          case "\u001B[0;33m":
              return D_Yellow;
          case "\u001B[0;34m":
              return D_Blue;
          case "\u001B[0;35m":
              return D_Magenta;
          case "\u001B[0;36m":
              return D_Cyan;
          case "\u001B[0;37m":
              return D_White;
          case "\u001B[1;30m":
              return B_Black;
          case "\u001B[1;31m":
              return B_Red;
          case "\u001B[1;32m":
              return B_Green;
          case "\u001B[1;33m":
              return B_Yellow;
          case "\u001B[1;34m":
              return B_Blue;
          case "\u001B[1;35m":
              return B_Magenta;
          case "\u001B[1;36m":
              return B_Cyan;
          case "\u001B[1;37m":
              return B_White;
          case "\u001B[0m":
              return cReset;
          default:
              return B_White;
      }
  }
}
