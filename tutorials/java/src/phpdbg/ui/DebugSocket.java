package phpdbg.ui;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import phpdbg.ui.JConsole.MessageType;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Manage input and output data
 * @author krakjoe
 */
public class DebugSocket extends Socket implements Runnable {
    private final Boolean reader;
    private final JConsole main;
    private final Thread thread;

    private volatile Boolean quit;
    private volatile Boolean started;
    
    public DebugSocket(final String host, final Integer port, final JConsole main, Boolean reader) throws IOException {
        super(host, port);
        
        this.main = main;
        this.reader = reader;
        this.quit = false;
        this.started = false;
        this.thread = new Thread(this);
    }
    
    public void start() {
        synchronized(this) {
            if (!started) {
               started = true;
               thread.start(); 
            }
        }
    }
    
    public void quit() {
        synchronized(this) {
            quit = true;
            started = false;
            notifyAll();
        }
    }
    
    @Override public void run() {
        try {
            synchronized(main) {
                if (!main.isConnected()) {
                    main.setConnected(true);
                }
            }
            
            synchronized(this) {
                do {
                    if (reader) {
                        String command;
                        OutputStream output = getOutputStream();
                        
                        wait();
                        
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
                            if (input.available() == 0) {
                                wait(400);
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
                main.messageBox(ex.getMessage(), MessageType.ERROR);
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
