<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Anders Johannsen <anders@johannsen.com>                      |
// +----------------------------------------------------------------------+
//
define('CMD_RCSID', '$Id$');

/**
 * The Cmd:: class implements an abstraction for various ways 
 * of executing commands (directly using the backtick operator,
 * as a background task after the script has terminated using
 * register_shutdown_function() or as a detached process using nohup).
 *
 * @author  Anders Johannsen <anders@johannsen.com>
 * @version $Revision$
 **/

require_once 'PEAR.php';


class Cmd extends PEAR
{
    var $arrSetting     = array();
    var $arrConstant    = array();
    var $arrCommand     = array();
        
    /**
     * Class constructor
     * 
     * Defines all necessary constants and sets defaults
     * 
     * @author Anders Johannsen <anders@johannsen.com>
     *
     * @access public
     * 
     **/
        
    function Cmd ()
    {
        // Defining constants
        $this->arrConstant  = array ("CMD_SEQUENCE",
                                     "CMD_SHUTDOWN",
                                     "CMD_SHELL",
                                     "CMD_OUTPUT",
                                     "CMD_NOHUP",
                                     "CMD_VERBOSE"
                                     );
                
        foreach ($this->arrConstant as $key => $value) {
            if (!defined($value)) {
                define($value, $key);
            }
        }
                
        // Setting default values
        $this->arrSetting[CMD_SEQUENCE]     = true; 
        $this->arrSetting[CMD_SHUTDOWN]     = false; 
        $this->arrSetting[CMD_OUTPUT]       = false; 
        $this->arrSetting[CMD_NOHUP]        = false; 
        $this->arrSetting[CMD_VERBOSE]      = false; 
                
        $arrShell = array ("sh", "bash", "zsh", "tcsh", "csh", "ash", "sash", "esh", "ksh");
                                   
        foreach ($arrShell as $shell) {
            if ($this->arrSetting[CMD_SHELL] = $this->which($shell)) {
                break;
            } 
        }
                
        if (empty($this->arrSetting[CMD_SHELL])) {
            $this->raiseError("No shell found");
        }
    }
        
    /**
     * Sets any option
     * 
     * The options are currently:
     * CMD_SHUTDOWN : Execute commands via a shutdown function 
     * CMD_SHELL    : Path to shell
     * CMD_OUTPUT   : Output stdout from process
     * CMD_NOHUP    : Use nohup to detach process
     * CMD_VERBOSE  : Print errors to stdout
     *
     * @param $option is a constant, which corresponds to the
     * option that should be changed
     *
     * @param $setting is the value of the option currently
     * being toggled.
     *
     * @return bool true if succes, else false
     *
     * @access public
     * 
     * @author Anders Johannsen <anders@johannsen.com>
     * 
     **/
        
    function setOption ($option, $setting)
    {
        if (empty($this->arrConstant[$option])) {
            $this->raiseError("No such option: $option");
            return false;
        }
    
                
        switch ($option) {
        case CMD_OUTPUT:
        case CMD_SHUTDOWN:
        case CMD_VERBOSE:
        case CMD_SEQUENCE:
            $this->arrSetting[$option] = $setting;
            return true;
            break;
            
        case CMD_SHELL:
            if (is_executable($setting)) {
                $this->arrSetting[$option] = $setting;
                return true;
            } else {
                $this->raiseError("No such shell: $setting");
                return false;
            }
            break;
            
                    
        case CMD_NOHUP:
            if  (empty($setting)) {
                $this->arrSetting[$option] = false;
                
            } else if ($location = $this->which("nohup")) {
                $this->arrSetting[$option] = true;
                
            } else {
                $this->raiseError("Nohup was not found on your system");
                return false;
            }
            break;
            
        }
    }
    
    /**
     * Add command for execution
     *
     * @param $command accepts both arrays and regular strings
     *
     * @return bool true if succes, else false
     *
     * @access public
     * 
     * @author Anders Johannsen <anders@johannsen.com>
     *
     **/
        
    function command($command) 
    {
        if (is_array($command)) {
            foreach ($command as $key => $value) {
                $this->arrCommand[] = $value;
            }
            return true;

        } else if (is_string($command)) {
            $this->arrCommand[] = $command;
            return true;
        }
                
        $this->raiseError("Argument not valid");
        return false;
    }
        
    /**
     * Executes the code according to given options
     *
     * @return bool true if succes, else false
     *
     * @access public
     * 
     * @author Anders Johannsen <anders@johannsen.com>
     *
     **/
        
    function exec() 
    {
        // Warning about impossible mix of options
        if (!empty($this->arrSetting[CMD_OUTPUT])) {
            if (!empty($this->arrSetting[CMD_SHUTDOWN]) || !empty($this->arrSetting[CMD_NOHUP])) {
                $this->raiseError("Error: Commands executed via shutdown functions or nohup cannot return output");
                return false;
            }
        }
                
        // Building command
        $strCommand = implode(";", $this->arrCommand);
                
        $strExec    = "echo '$strCommand' | ".$this->arrSetting[CMD_SHELL];
                
        if (empty($this->arrSetting[CMD_OUTPUT])) {
            $strExec = $strExec . ' > /dev/null';
        }
                
        if (!empty($this->arrSetting[CMD_NOHUP])) {
            $strExec = 'nohup ' . $strExec;
        }
                
        // Executing 
        if (!empty($this->arrSetting[CMD_SHUTDOWN])) {
            $line = "system(\"$strExec\");";
            $function = create_function('', $line);
            register_shutdown_function($function);
            return true;
        } else {
            return `$strExec`;
        }
    }

    /**
     * Errorhandler. If option CMD_VERBOSE is true,
     * the error is printed to stdout, otherwise it 
     * is avaliable in lastError
     *
     * @return bool always returns true 
     *
     * @access private
     * 
     * @author Anders Johannsen <anders@johannsen.com>
     **/
        
    function raiseError($strError) 
    {
        if (!empty($this->arrSetting[CMD_VERBOSE])) {
            echo $strError;
        } else {
            $this->lastError = $strError;
        }
        
        return true;
    }
        
    /**
     * Functionality similiar to unix 'which'. Searches the path
     * for the specified program. 
     *
     * @param $cmd name of the executable to search for 
     *
     * @return string returns the full path if found,
     * false if not
     *
     * @access private
     * 
     * @author Anders Johannsen <anders@johannsen.com>
     **/
        
    function which($cmd) 
    {
        global $HTTP_ENV_VARS;
                
        $arrPath = explode(":", $HTTP_ENV_VARS['PATH']);
            
        foreach ($arrPath as $path) {
            $location = $path . "/" . $cmd;
                            
            if (is_executable($location)) {
                return $location;
            }
        }
        return false;
    }   
}
    
?>
