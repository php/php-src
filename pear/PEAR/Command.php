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
// | Author: Stig Bakken <ssb@php.net>                                    |
// +----------------------------------------------------------------------+
//
// $Id$


require_once "PEAR.php";

/**
 * List of commands and what classes they are implemented in.
 * @var array command => implementing class
 */
$GLOBALS['_PEAR_Command_commandlist'] = array();

/**
 * List of shortcuts to common commands.
 * @var array shortcut => command
 */
$GLOBALS['_PEAR_Command_shortcuts'] = array();

/**
 * Array of command objects
 * @var array class => object
 */
$GLOBALS['_PEAR_Command_objects'] = array();

/**
 * Which user interface class is being used.
 * @var string class name
 */
$GLOBALS['_PEAR_Command_uiclass'] = 'PEAR_Frontend_CLI';

/**
 * Instance of $_PEAR_Command_uiclass.
 * @var object
 */
$GLOBALS['_PEAR_Command_uiobject'] = null;

/**
 * PEAR command class, a simple factory class for administrative
 * commands.
 *
 * How to implement command classes:
 *
 * - The class must be called PEAR_Command_Nnn, installed in the
 *   "PEAR/Common" subdir, with a method called getCommands() that
 *   returns an array of the commands implemented by the class (see
 *   PEAR/Command/Install.php for an example).
 *
 * - The class must implement a run() function that is called with three
 *   params:
 *
 *    (string) command name
 *    (array)  assoc array with options, freely defined by each
 *             command, for example:
 *             array('force' => true)
 *    (array)  list of the other parameters
 *
 *   The run() function returns a PEAR_CommandResponse object.  Use
 *   these methods to get information:
 *
 *    int getStatus()   Returns PEAR_COMMAND_(SUCCESS|FAILURE|PARTIAL)
 *                      *_PARTIAL means that you need to issue at least
 *                      one more command to complete the operation
 *                      (used for example for validation steps).
 *
 *    string getMessage()  Returns a message for the user.  Remember,
 *                         no HTML or other interface-specific markup.
 *
 *   If something unexpected happens, run() returns a PEAR error.
 *
 * - DON'T OUTPUT ANYTHING! Return text for output instead.
 *
 * - DON'T USE HTML! The text you return will be used from both Gtk,
 *   web and command-line interfaces, so for now, keep everything to
 *   plain text.
 *
 * - DON'T USE EXIT OR DIE! Always use pear errors.  From static
 *   classes do PEAR::raiseError(), from other classes do
 *   $this->raiseError().
 */
class PEAR_Command
{
    /**
     * Get the right object for executing a command.
     *
     * @param string $command The name of the command
     * @param object $config  Instance of PEAR_Config object
     *
     * @return object the command object or a PEAR error
     *
     * @access public
     */
    function factory($command, &$config)
    {
        if (empty($GLOBALS['_PEAR_Command_commandlist'])) {
            PEAR_Command::registerCommands();
        }
        if (isset($GLOBALS['_PEAR_Command_shortcuts'][$command])) {
            $command = $GLOBALS['_PEAR_Command_shortcuts'][$command];
        }
        $class = @$GLOBALS['_PEAR_Command_commandlist'][$command];
        if (empty($class)) {
            return PEAR::raiseError("unknown command `$command'");
        }
        $ui = PEAR_Command::getFrontendObject();
        $obj = &new $class($ui, $config);
        return $obj;
    }

    /**
     * Get instance of frontend object.
     *
     * @return object
     */
    function &getFrontendObject()
    {
        if (empty($GLOBALS['_PEAR_Command_uiobject'])) {
            $GLOBALS['_PEAR_Command_uiobject'] = &new $GLOBALS['_PEAR_Command_uiclass'];
        }
        return $GLOBALS['_PEAR_Command_uiobject'];
    }

    /**
     * Load current frontend class.
     *
     * @param string $uiclass Name of class implementing the frontend
     *
     * @return object the frontend object, or a PEAR error
     */
    function &setFrontendClass($uiclass)
    {
        if (is_object($GLOBALS['_PEAR_Command_uiobject']) &&
            strtolower($uiclass) == get_class($GLOBALS['_PEAR_Command_uiobject'])) {
            return;
        }
        $file = str_replace('_', '/', $uiclass) . '.php';
        @include_once $file;
        if (class_exists(strtolower($uiclass))) {
            $obj = &new $uiclass;
            // quick test to see if this class implements a few of the most
            // important frontend methods
            if (method_exists($obj, 'userConfirm')) {
                $GLOBALS['_PEAR_Command_uiobject'] = &$obj;
                $GLOBALS['_PEAR_Command_uiclass'] = $uiclass;
                return $obj;
            } else {
                return PEAR::raiseError("not a frontend class: $uiclass");
            }
        }
        return PEAR::raiseError("no such class: $uiclass");
    }

    /**
     * Set current frontend.
     *
     * @param string $uitype Name of the frontend type (for example "CLI")
     *
     * @return object the frontend object, or a PEAR error
     */
    function setFrontendType($uitype)
    {
        $uiclass = 'PEAR_Frontend_' . $uitype;
        return PEAR_Command::setFrontendClass($uiclass);
    }

    /**
     * Scan through the Command directory looking for classes
     * and see what commands they implement.
     *
     * @param bool   (optional) if FALSE (default), the new list of
     *               commands should replace the current one.  If TRUE,
     *               new entries will be merged with old.
     *
     * @param string (optional) where (what directory) to look for
     *               classes, defaults to the Command subdirectory of
     *               the directory from where this file (__FILE__) is
     *               included.
     *
     * @return bool TRUE on success, a PEAR error on failure
     *
     * @access public
     */
    function registerCommands($merge = false, $dir = null)
    {
        if ($dir === null) {
            $dir = dirname(__FILE__) . '/Command';
        }
        $dp = @opendir($dir);
        if (empty($dp)) {
            return PEAR::raiseError("registerCommands: opendir($dir) failed");
        }
        if (!$merge) {
            $GLOBALS['_PEAR_Command_commandlist'] = array();
        }
        while ($entry = readdir($dp)) {
            if ($entry{0} == '.' || substr($entry, -4) != '.php' || $entry == 'Common.php') {
                continue;
            }
            $class = "PEAR_Command_".substr($entry, 0, -4);
            $file = "$dir/$entry";
            include_once $file;
            // List of commands
            if (empty($GLOBALS['_PEAR_Command_objects'][$class])) {
                $GLOBALS['_PEAR_Command_objects'][$class] = &new $class($ui, $config);
            }
            $implements = $GLOBALS['_PEAR_Command_objects'][$class]->getCommands();
            foreach ($implements as $command => $desc) {
                $GLOBALS['_PEAR_Command_commandlist'][$command] = $class;
                $GLOBALS['_PEAR_Command_commanddesc'][$command] = $desc;
            }
            $shortcuts = $GLOBALS['_PEAR_Command_objects'][$class]->getShortcuts();
            foreach ($shortcuts as $shortcut => $command) {
                $GLOBALS['_PEAR_Command_shortcuts'][$shortcut] = $command;
            }
        }
        return true;
    }

    /**
     * Get the list of currently supported commands, and what
     * classes implement them.
     *
     * @return array command => implementing class
     *
     * @access public
     */
    function getCommands()
    {
        if (empty($GLOBALS['_PEAR_Command_commandlist'])) {
            PEAR_Command::registerCommands();
        }
        return $GLOBALS['_PEAR_Command_commandlist'];
    }

    /**
     * Get the list of command shortcuts.
     *
     * @return array shortcut => command
     *
     * @access public
     */
    function getShortcuts()
    {
        if (empty($GLOBALS['_PEAR_Command_shortcuts'])) {
            PEAR_Command::registerCommands();
        }
        return $GLOBALS['_PEAR_Command_shortcuts'];
    }

    /**
     * Compiles arguments for getopt.
     *
     * @param string $command     command to get optstring for
     * @param string $short_args  (reference) short getopt format
     * @param array  $long_args   (reference) long getopt format
     *
     * @return void
     *
     * @access public
     */
    function getGetoptArgs($command, &$short_args, &$long_args)
    {
        if (empty($GLOBALS['_PEAR_Command_commandlist'])) {
            PEAR_Command::registerCommands();
        }
        $class = @$GLOBALS['_PEAR_Command_commandlist'][$command];
        if (empty($class)) {
            return null;
        }
        $obj = &$GLOBALS['_PEAR_Command_objects'][$class];
        return $obj->getGetoptArgs($command, $short_args, $long_args);
    }

    /**
     * Get description for a command.
     *
     * @param  string $command Name of the command
     *
     * @return string command description
     *
     * @access public
     */
    function getDescription($command)
    {
        return @$GLOBALS['_PEAR_Command_commanddesc'][$command];
    }

    /**
     * Get help for command.
     *
     * @param string $command Name of the command to return help for
     *
     * @access public
     */
    function getHelp($command)
    {
        $cmds = PEAR_Command::getCommands();
        if (isset($cmds[$command])) {
            $class = $cmds[$command];
            return $GLOBALS['_PEAR_Command_objects'][$class]->getHelp($command);
        }
        return false;
    }
}

?>
