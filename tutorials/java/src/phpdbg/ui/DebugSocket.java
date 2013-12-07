package phpdbg.ui;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import static java.nio.channels.SelectionKey.OP_READ;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;
import java.util.Iterator;
import java.util.Set;
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
public class DebugSocket implements Runnable {
    private final String host;
    private final Integer port;
    private final Boolean reader;
    private final JConsole main;
    private final Thread thread;

    private volatile Boolean quit;
    private volatile Boolean started;
    
    public DebugSocket(final String host, final Integer port, final JConsole main, Boolean reader) throws IOException {
        this.host = host;
        this.port = port;
        this.main = main;
        this.reader = reader;
        this.quit = false;
        this.started = false;
        this.thread = new Thread(this);
    }
    
    public void start() {
        synchronized(this) {
            if (!started) {
               quit = false;
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
            
            if (reader) {
                /* The reader thread will wait() until there is input */
                Socket socket = new Socket(this.host, this.port);
                String command;
                OutputStream output = socket.getOutputStream();
                
                do {
                    synchronized(this) {
                        wait();

                        if (!quit) {
                            command = main.getInputField().getText();
                            /* send command to stdin socket */
                            if (command != null) {
                                if (main.isEchoing()) {
                                    main.getOutputField()
                                           .appendANSI(
                                                   String.format("remote> %s\n", command));
                                }
                                output.write(
                                   command.getBytes());
                                output.write("\n".getBytes());
                                output.flush();
                            }
                            main.getInputField().setText(null);
                        }
                    }
                } while (!quit);
                
                socket.close();
            } else {
                /*
                * The writer thread will use non-blocking i/o consuming
                * resources only when there is data to read
                */
                Selector selector = Selector.open();
                SocketChannel channel = SocketChannel.open();
                
                channel.connect(
                        new InetSocketAddress(this.host, this.port));
                channel.configureBlocking(false);
                channel.register(selector, OP_READ);

                while (!quit) {
                    selector.select();

                    Iterator<SelectionKey> iter = selector.selectedKeys().iterator();

                    while (iter.hasNext()) {
                        if (!quit) {
                            SocketChannel ready = (SocketChannel) (iter.next().channel());
                            ByteBuffer bytes = ByteBuffer.allocate(128);
                            
                            if (ready != null) {
                                if (ready.read(bytes) != -1) {
                                    bytes.flip();
                                    
                                    main.getOutputField()
                                        .appendANSI(new String(bytes.array()));
                                    
                                }
                            }

                            iter.remove();
                        }
                    }
                }
                channel.close();
            }
        } catch (IOException | InterruptedException ex) {
            if (!quit) {
                main.messageBox(ex.getMessage(), MessageType.ERROR);
            }
        } finally {
            synchronized(main) {
                if (main.isConnected()) {
                    main.setConnected(false);
                }
            }
        }
    }
}
