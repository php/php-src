<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Chuck Hagenbuch <chuck@horde.org>                           |
// +----------------------------------------------------------------------+
//
// $Id$
//

/**
 * Generalized socket class, in roughly the same vein as the perl
 * IO::Socket package.
 *
 * @author  Chuck Hagenbuch <chuck@horde.org>
 * @version $Revision$
 */
class Socket {
    
    // {{{ properties
    
    /** Number of bytes to read at a time in readline(). */
    var $len = 2048;
	
    /** Number of seconds to wait on socket connections before
        assuming there's no more data. */
    var $timeout = 100;
    
    /** The hostname or IP address to connect to. */
    var $host = '';
    
    /** The port number to connect to. */
    var $port = 0;
    
    /** The socket handle. */
    var $fp = 0;
    
    /** The number of the most recent error. */
    var $errno  = 0;
    
    /** The message associated with the most recent error. */
    var $errstr = '';
    
	// }}}
    
    
    // {{{ constructor
    /**
     * Constructs a new Socket object.
     * 
     * @param $host The hostname or IP address to connect to.
     * @param $port The port number to connect to.
     */
    function Socket ($host, $port) {
        $this->host = $host;
        $this->port = $port;
    }
	// }}}
    
    
	// {{{ open()
    /**
     * Attempt to open the socket connection.
     *
     * @return true on success, false on an error.
     */
    function open () {
        $this->fp = fsockopen($this->host, $this->port, $this->errno, $this->errstr);
        if (!$this->fp)
            return false;
        else
            return true;
    }
	// }}}
    
	// {{{ close()
    /**
     * Closes the socket connection, if it is opened.
     */
    function close () {
        @fclose($this->fp);
    }
	// }}}
    
    // {{{ send()
    /**
     * Write a line of data to the socket, followed by a trailing "\r\n".
     *
     * @return The fputs result.
     */
    function send ($data) {
        return fputs($this->fp, "$data\r\n");
    }
	// }}}
    
	// {{{ sendraw()
    /**
     * Write data to the socket with no modifications - no newlines
     * are appended, etc.. 
     *
     * @return The fputs result.
     */
    function sendraw ($data) {
        return fputs($this->fp, "$data");
    }
	// }}}
    
	// {{{ error()
    /**
     * Convert the last error into a printable form.
     *
     * @return A string representation of an error message.
     */
    function error () {
        return "$this->errstr ($this->errno)";
    }
	// }}}
    
	// {{{ blocking()
    /**
     * Set the blocking to $mode. $mode can be anything which evaluates
     * to true or false.
     */
    function blocking ($mode) {
        set_socket_blocking($this->fp, $mode);
    }
	// }}}
    
	// {{{ readline()
    /**
     * Read until either the end of the socket or a newline, whichever
     * comes first. Strips the trailing newline from the returned data.
     *
     * @return All available data up to a newline, without that
     *         newline, or until the end of the socket.
     */
    function readline () {
        $line = '';
        $timeout = time() + $this->timeout;
        while (!feof($this->fp) && time() < $timeout) {
            $line .= fgets($this->fp, $this->len);
            $len = strlen($line);
            if ($len >=2 && substr($line, $len-2, 2) == "\r\n")
                return substr($line, 0, $len-2);
        }
        return $line;
    }
	// }}}
    
	// {{{ readall()
    /**
     * Read until the socket closes. THIS FUNCTION WILL NOT EXIT if the
     * socket is in blocking mode until the socket closes.
     *
     * @return All data until the socket closes.
     */
    function readall () {
        $data = '';
        while (!feof($this->fp))
            $data .= fread($this->fp, $this->len);
        return $data;
    }
	// }}}
    
	// {{{ readlen()
    /**
     * Read a specified amount of data. This is guaranteed to return,
     * and has the added benefit of getting everything in one fread()
     * chunk; if you know the size of the data you're getting
     * beforehand, this is definitely the way to go.
     *
     * @param $len The number of bytes to read from the socket.
     *
     * @return $len bytes of data from the socket.
     */
    function readlen ($len) {
        return fread($this->fp, $len);
    }
	// }}}
    
}

?>
