package phpdbg.ui;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Manage input and output data
 * @author krakjoe
 */
public class DBGThread extends Socket implements Runnable {
    private final Boolean reader;
    private final Main main;
    private Boolean quit;

    public DBGThread(final String host, final Integer port, final Main main, Boolean reader) throws IOException {
        super(host, port);
        
        this.main = main;
        this.reader = reader;
        this.quit = false;
        
        synchronized(main) {
            if (!main.isConnected()) {
                main.setConnected(true);
            }
        }
    }
    
    public void quit() {
        synchronized(this) {
            quit = true;
            this.notifyAll(); 
        }
    }
    
    @Override public void run() {
        try {
            synchronized(this) {
                do {
                    if (reader) {
                        String command;
                        OutputStream output = getOutputStream();
                        
                        this.wait();
                        
                        command = main.getInputField().getText();
                        /* send command to stdin socket */
                        if (command != null) {    
                            output.write(
                               command.getBytes());
                            output.write("\n".getBytes());
                            output.flush();
                        }
                        main.getInputField().setText(null);
                    } else {
                        InputStream input = getInputStream();
                        /* get data from stdout socket */
                        byte[] bytes = new byte[1];
                        do {
                            /* this is some of the laziest programming I ever done */
                            if (input.available() == 0) {
                                this.wait(400);
                                continue;
                            }
                            
                            if (input.read(bytes, 0, 1) > -1) {
                                main.getOutputField()
                                        .appendANSI(new String(bytes));
                            }
                        } while (!quit);
                    }
                } while(!quit);
            }
        } catch (IOException | InterruptedException ex) {
            if (!quit) {
                main.messageBox(ex.getMessage());
            }
        } finally {
            try {
                close();
            } catch (IOException ex) { /* naughty me */ } finally {
               synchronized(main) {
                   if (main.isConnected()) {
                       main.setConnected(false);
                   }
               } 
            }
        }
    }
}
