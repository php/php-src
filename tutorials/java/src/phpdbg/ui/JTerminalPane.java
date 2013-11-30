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
import java.util.HashMap;
import javax.imageio.ImageIO;

public class JTerminalPane extends JTextPane {
    private static class JTerminalColor {
        private final Color color;
        private final Boolean bold;
        private final Boolean underline;

        public JTerminalColor(Float h, Float s, Float b, Boolean bold, Boolean underline) {
            this.color = Color.getHSBColor(h, s, b);
            this.bold = bold;
            this.underline = underline;
        }
        
        public JTerminalColor(Float h, Float s, Float b, Boolean bold) {
            this(h, s, b, bold, false);
        }
        
        public JTerminalColor(Float h, Float s, Float b) {
            this(h, s, b, false, false);
        }

        public Boolean      isUnderlined()  { return this.underline; }
        public Boolean      isBold()        { return this.bold; }
        public Color        getColor()      { return this.color; }
    }

    private static class JTerminalColors extends HashMap<String, JTerminalColor> {
        public JTerminalColors() {
            put("\u001B[0;30m", new JTerminalColor(0.000f, 0.000f, 0.502f));
            put("\u001B[0;31m", new JTerminalColor(0.000f, 1.000f, 1.000f));
            put("\u001B[0;32m", new JTerminalColor(0.333f, 1.000f, 1.000f));
            put("\u001B[0;33m", new JTerminalColor(0.167f, 1.000f, 1.000f));
            put("\u001B[0;34m", new JTerminalColor(0.667f, 1.000f, 1.000f));
            put("\u001B[0;35m", new JTerminalColor(0.833f, 1.000f, 1.000f));
            put("\u001B[0;36m", new JTerminalColor(0.500f, 1.000f, 1.000f));
            put("\u001B[0;37m", new JTerminalColor(0.000f, 0.000f, 1.000f));
            put("\u001B[0;64m", new JTerminalColor(0.000f, 0.000f, 1.000f));
            put("\u001B[1;30m", new JTerminalColor(0.000f, 0.000f, 0.502f, true));
            put("\u001B[1;31m", new JTerminalColor(0.000f, 1.000f, 1.000f, true));
            put("\u001B[1;32m", new JTerminalColor(0.333f, 1.000f, 1.000f, true));
            put("\u001B[1;33m", new JTerminalColor(0.167f, 1.000f, 1.000f, true));
            put("\u001B[1;34m", new JTerminalColor(0.667f, 1.000f, 1.000f, true));
            put("\u001B[1;35m", new JTerminalColor(0.833f, 1.000f, 1.000f, true));
            put("\u001B[1;36m", new JTerminalColor(0.500f, 1.000f, 1.000f, true));
            put("\u001B[1;37m", new JTerminalColor(0.000f, 0.000f, 1.000f, true));
            put("\u001B[1;64m", new JTerminalColor(0.000f, 0.000f, 1.000f, true));
            put("\u001B[4;30m", new JTerminalColor(0.000f, 0.000f, 0.502f, false, true));
            put("\u001B[4;31m", new JTerminalColor(0.000f, 1.000f, 1.000f, false, true));
            put("\u001B[4;32m", new JTerminalColor(0.333f, 1.000f, 1.000f, false, true));
            put("\u001B[4;33m", new JTerminalColor(0.167f, 1.000f, 1.000f, false, true));
            put("\u001B[4;34m", new JTerminalColor(0.667f, 1.000f, 1.000f, false, true));
            put("\u001B[4;35m", new JTerminalColor(0.833f, 1.000f, 1.000f, false, true));
            put("\u001B[4;36m", new JTerminalColor(0.500f, 1.000f, 1.000f, false, true));
            put("\u001B[4;37m", new JTerminalColor(0.000f, 0.000f, 1.000f, false, true));
            put("\u001B[4;64m", new JTerminalColor(0.000f, 0.000f, 1.000f, false, true));
            
            put("reset", new JTerminalColor(0.000f, 0.000f, 1.000f));
        }
        
        public JTerminalColor find(String ANSIColor) {
            if (containsKey(ANSIColor)) {
                return get(ANSIColor);
            } else return get("reset");
        }
    }
    
    public JTerminalPane() {
        super();
        setOpaque(false);
        setBackground(new Color(0, 0, 0, 0));
        colorCurrent = colors.find("reset");
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

    private void append(JTerminalColor c, String s) {
        StyleContext sc = StyleContext.getDefaultStyleContext();
        AttributeSet aset = sc.addAttribute(
                SimpleAttributeSet.EMPTY, StyleConstants.Foreground, c.getColor());
        
        aset = sc.addAttribute(aset, StyleConstants.Underline, c.isUnderlined());
        aset = sc.addAttribute(aset, StyleConstants.Bold, c.isBold());

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
                    colorCurrent = colors.find(tmpString);
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
    
    private static JTerminalColors colors = new JTerminalColors();
    private JTerminalColor colorCurrent;
    private String remaining = "";
}
