
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * In a RUSH !!!
 * @author joe
 */
public class DBGThread extends Socket implements Runnable {
    private final String host;
    private final Integer port;
    private final Boolean reader;
    private final JTextField field;
    private final JTextArea area;
    private final JScrollPane pane;
    private InputStream input;
    private OutputStream output;
    private String buffer;
    private Boolean quit;

    public DBGThread(final String host, final Integer port, final JTextField field) throws IOException {
        super(host, port);
        
        this.host = host;
        this.port = port;
        this.reader = true;
        this.field = field;
        this.area = null;
        this.pane = null;
        this.quit = false;
        this.buffer = null;
    }
    
    public DBGThread(final String host, final Integer port, final JTextArea area, JScrollPane pane) throws IOException {
        super(host, port);
        
        this.host = host;
        this.port = port;
        this.reader = false;
        this.field = null;
        this.area = area;
        this.pane = pane;
        this.quit = false;
        this.buffer = null;
    }
    
    public Boolean isReader() {
        return this.reader;
    }
    
    public void quit() {
        synchronized(this) {
            this.quit = true;
            this.notifyAll(); 
        }
    }
    
    @Override
    public void run() {
        try {
            if (this.reader) {
                output = this.getOutputStream();
            } else input = this.getInputStream();
            
            synchronized(this) {
                do {
                    if (this.reader) {
                        this.wait();
                        
                        /* send command to stdin socket */
                        if (this.field.getText() != null) {    
                            output.write(
                               this.field.getText().getBytes());
                            output.write("\n".getBytes());
                            output.flush();
                        }
                        this.field.setText(null);
                        
                    } else {
                        /* get data from stdout socket */
                        byte[] bytes = new byte[1];
                        do {
                            /* this is some of the laziest programming I ever done */
                            if (input.available() == 0) {
                                this.wait(666);
                                continue;
                            }
                            
                            if (input.read(bytes, 0, 1) > -1) {
                                area.setCaretPosition(
                                        area.getText().length());
                                area.append(new String(bytes));
                            }
                        } while (!this.quit);
                    }
                } while(!this.quit);
            }
        } catch (IOException | InterruptedException ex) {
            Logger.getLogger(DBGThread.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
