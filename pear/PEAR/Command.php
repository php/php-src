<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Stig Bakken <ssb@fast.no>                                    |
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
 * Which user interface class is being used.
 * @var string class name
 */
$GLOBALS['_PEAR_Command_uiclass'] = 'PEAR_CommandUI_CLI';

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
 *   web and command-line interfaces, so for now keep everything to
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
     * @param object Instance of PEAR_Config object
     * @param string The name of the command
     *
     * @return object the command object or a PEAR error
     *
     * @access public
     */
    function factory($command)
    {
        if (empty($GLOBALS['_PEAR_Command_commandlist'])) {
            PEAR_Command::registerCommands();
        }
        if (isset($GLOBALS['_PEAR_Command_commandlist'][$command])) {
            $class = $GLOBALS['_PEAR_Command_commandlist'][$command];
            $obj = &new $class(PEAR_Command::getUIObject());
            return $obj;
        }
        return PEAR::raiseError("unknown command `$command'");
    }

    function &getUIObject()
    {
        global $_PEAR_Command_uiclass, $_PEAR_Command_uiobject;
        if (empty($_PEAR_Command_uiobject)) {
            $_PEAR_Command_uiobject = &new $_PEAR_Command_uiclass;
        }
        return $_PEAR_Command_uiobject;
    }

    function setUIClass($uiclass)
    {
        $GLOBALS['_PEAR_Command_uiclass'] = $uiclass;
        $file = str_replace("_", "/", $uiclass) . '.php';
        include_once $file;
        return class_exists(strtolower($uiclass));
    }

    function setUIType($uitype)
    {
        $uiclass = 'PEAR_CommandUI_' . $uitype;
        return PEAR_Command::setUIClass($uiclass);
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
            return PEAR::raiseError("PEAR_Command::registerCommands: ".
                                    "opendir($dir) failed");
        }
        if (!$merge) {
            $GLOBALS['_PEAR_Command_commandlist'] = array();
        }
        while ($entry = readdir($dp)) {
            if ($entry{0} == '.' || substr($entry, -4) != '.php' ||
                $entry == 'Common.php')
            {
                continue;
            }
            $class = "PEAR_Command_".substr($entry, 0, -4);
            $file = "$dir/$entry";
            include_once $file;
            $implements = call_user_func(array($class, "getCommands"));
            foreach ($implements as $command) {
                $GLOBALS['_PEAR_Command_commandlist'][$command] = $class;
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
}

?>