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

require_once "PEAR.php";

$GLOBALS['_PEAR_Command_commandlist'] = array();

class PEAR_Command
{
    function factory($command)
    {
        static $command_classes = array(
            'install'   => 'PEAR_Command_Install':
            'uninstall' => 'PEAR_Command_Install':
            'upgrade'   => 'PEAR_Command_Install':
            );
        if (isset($command_classes[$command])) {
            $class = $command_classes[$command];
            $obj =& new $class();
            return $obj;
        }
        return PEAR::raiseError("unknown command: $command");
    }

    function registerAllCommands()
    {
        $dir = dirname(__FILE__) . '/Command';
        $dp = @opendir($dir);
        if (empty($dp)) {
            return PEAR::raiseError("PEAR_Command::registerAllCommands: opendir($dir) failed");
        }
        while ($entry = readdir($dp)) {
            if ($entry{0} == '.' || substr($entry, -4) != '.php' || $entry == 'Common.php') {
                continue;
            }
            $class = "PEAR_Command_".substr($entry, 0, -4);
            $file = "$dir/$entry";
            include_once $file;
            $implements = call_user_func(array($class, "getCommands"));
        }
    }
}

?>