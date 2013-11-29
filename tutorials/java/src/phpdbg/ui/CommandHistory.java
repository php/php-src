package phpdbg.ui;


import java.util.ArrayList;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Implement a simple history list for command input
 * @author krakjoe
 */
public class CommandHistory extends ArrayList<String> {
    private Integer position = new Integer(0);
    
    public CommandHistory() {
        super();
    }
    
    @Override public boolean add(String text) {
        String last = last();
        if (text != null) {
           if (last == null || !last.equals(text)) {
               if (super.add(text)) {
                   position = size();
                   return true;
               }
           }
        }
        return false;
    }
    
    public String last() {
        if (position >= 1) {
            position--;
            return get(position);
        } else return new String();
    }
    
    public String next() {
        if (position+1 < size()) {
            position++;
            return get(position);
        } else return new String();
    }
}