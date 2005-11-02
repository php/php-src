<?php
/**
 * PEAR_Command, command pattern class
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * Needed for error handling
 */
require_once 'PEAR.php';
require_once 'PEAR/Frontend.php';
require_once 'PEAR/XMLParser.php';

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
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
 */
class PEAR_Command
{
    // {{{ factory()

    /**
     * Get the right object for executing a command.
     *
     * @param string $command The name of the command
     * @param object $config  Instance of PEAR_Config object
     *
     * @return object the command object or a PEAR error
     *
     * @access public
     * @static
     */
    function &factory($command, &$config)
    {
        if (empty($GLOBALS['_PEAR_Command_commandlist'])) {
            PEAR_Command::registerCommands();
        }
        if (isset($GLOBALS['_PEAR_Command_shortcuts'][$command])) {
            $command = $GLOBALS['_PEAR_Command_shortcuts'][$command];
        }
        if (!isset($GLOBALS['_PEAR_Command_commandlist'][$command])) {
            $a = PEAR::raiseError("unknown command `$command'");
            return $a;
        }
        $class = $GLOBALS['_PEAR_Command_commandlist'][$command];
        if (!class_exists($class)) {
            require_once $GLOBALS['_PEAR_Command_objects'][$class];
        }
        if (!class_exists($class)) {
            $a = PEAR::raiseError("unknown command `$command'");
            return $a;
        }
        $ui =& PEAR_Command::getFrontendObject();
        $obj = &new $class($ui, $config);
        return $obj;
    }

    // }}}
    // {{{ & getObject()
    function &getObject($command)
    {
        $class = $GLOBALS['_PEAR_Command_commandlist'][$command];
        if (!class_exists($class)) {
            require_once $GLOBALS['_PEAR_Command_objects'][$class];
        }
        if (!class_exists($class)) {
            return PEAR::raiseError("unknown command `$command'");
        }
        $ui =& PEAR_Command::getFrontendObject();
        $config = &PEAR_Config::singleton();
        $obj = &new $class($ui, $config);
        return $obj;
    }

    // }}}
    // {{{ & getFrontendObject()

    /**
     * Get instance of frontend object.
     *
     * @return object|PEAR_Error
     * @static
     */
    function &getFrontendObject()
    {
        $a = &PEAR_Frontend::singleton();
        return $a;
    }

    // }}}
    // {{{ & setFrontendClass()

    /**
     * Load current frontend class.
     *
     * @param string $uiclass Name of class implementing the frontend
     *
     * @return object the frontend object, or a PEAR error
     * @static
     */
    function &setFrontendClass($uiclass)
    {
        $a = &PEAR_Frontend::setFrontendClass($uiclass);
        return $a;
    }

    // }}}
    // {{{ setFrontendType()

    /**
     * Set current frontend.
     *
     * @param string $uitype Name of the frontend type (for example "CLI")
     *
     * @return object the frontend object, or a PEAR error
     * @static
     */
    function setFrontendType($uitype)
    {
        $uiclass = 'PEAR_Frontend_' . $uitype;
        return PEAR_Command::setFrontendClass($uiclass);
    }

    // }}}
    // {{{ registerCommands()

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
     * @static
     */
    function registerCommands($merge = false, $dir = null)
    {
        $parser = new PEAR_XMLParser;
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
            if ($entry{0} == '.' || substr($entry, -4) != '.xml') {
                continue;
            }
            $class = "PEAR_Command_".substr($entry, 0, -4);
            $file = "$dir/$entry";
            $parser->parse(file_get_contents($file));
            $implements = $parser->getData();
            // List of commands
            if (empty($GLOBALS['_PEAR_Command_objects'][$class])) {
                $GLOBALS['_PEAR_Command_objects'][$class] = "$dir/" . substr($entry, 0, -4) .
                    '.php';
            }
            foreach ($implements as $command => $desc) {
                if ($command == 'attribs') {
                    continue;
                }
                if (isset($GLOBALS['_PEAR_Command_commandlist'][$command])) {
                    return PEAR::raiseError('Command "' . $command . '" already registered in ' .
                        'class "' . $GLOBALS['_PEAR_Command_commandlist'][$command] . '"');
                }
                $GLOBALS['_PEAR_Command_commandlist'][$command] = $class;
                $GLOBALS['_PEAR_Command_commanddesc'][$command] = $desc['summary'];
                if (isset($desc['shortcut'])) {
                    $shortcut = $desc['shortcut'];
                    if (isset($GLOBALS['_PEAR_Command_shortcuts'][$shortcut])) {
                        return PEAR::raiseError('Command shortcut "' . $shortcut . '" already ' .
                            'registered to command "' . $command . '" in class "' .
                            $GLOBALS['_PEAR_Command_commandlist'][$command] . '"');
                    }
                    $GLOBALS['_PEAR_Command_shortcuts'][$shortcut] = $command;
                }
                if (isset($desc['options']) && $desc['options']) {
                    foreach ($desc['options'] as $oname => $option) {
                        if (isset($option['shortopt']) && strlen($option['shortopt']) > 1) {
                            return PEAR::raiseError('Option "' . $oname . '" short option "' .
                                $option['shortopt'] . '" must be ' .
                                'only 1 character in Command "' . $command . '" in class "' .
                                $class . '"');
                        }
                    }
                }
            }
        }
        ksort($GLOBALS['_PEAR_Command_shortcuts']);
        ksort($GLOBALS['_PEAR_Command_commandlist']);
        @closedir($dp);
        return true;
    }

    // }}}
    // {{{ getCommands()

    /**
     * Get the list of currently supported commands, and what
     * classes implement them.
     *
     * @return array command => implementing class
     *
     * @access public
     * @static
     */
    function getCommands()
    {
        if (empty($GLOBALS['_PEAR_Command_commandlist'])) {
            PEAR_Command::registerCommands();
        }
        return $GLOBALS['_PEAR_Command_commandlist'];
    }

    // }}}
    // {{{ getShortcuts()

    /**
     * Get the list of command shortcuts.
     *
     * @return array shortcut => command
     *
     * @access public
     * @static
     */
    function getShortcuts()
    {
        if (empty($GLOBALS['_PEAR_Command_shortcuts'])) {
            PEAR_Command::registerCommands();
        }
        return $GLOBALS['_PEAR_Command_shortcuts'];
    }

    // }}}
    // {{{ getGetoptArgs()

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
     * @static
     */
    function getGetoptArgs($command, &$short_args, &$long_args)
    {
        if (empty($GLOBALS['_PEAR_Command_commandlist'])) {
            PEAR_Command::registerCommands();
        }
        if (isset($GLOBALS['_PEAR_Command_shortcuts'][$command])) {
            $command = $GLOBALS['_PEAR_Command_shortcuts'][$command];
        }
        if (!isset($GLOBALS['_PEAR_Command_commandlist'][$command])) {
            return null;
        }
        $obj = &PEAR_Command::getObject($command);
        return $obj->getGetoptArgs($command, $short_args, $long_args);
    }

    // }}}
    // {{{ getDescription()

    /**
     * Get description for a command.
     *
     * @param  string $command Name of the command
     *
     * @return string command description
     *
     * @access public
     * @static
     */
    function getDescription($command)
    {
        if (!isset($GLOBALS['_PEAR_Command_commanddesc'][$command])) {
            return null;
        }
        return $GLOBALS['_PEAR_Command_commanddesc'][$command];
    }

    // }}}
    // {{{ getHelp()

    /**
     * Get help for command.
     *
     * @param string $command Name of the command to return help for
     *
     * @access public
     * @static
     */
    function getHelp($command)
    {
        $cmds = PEAR_Command::getCommands();
        if (isset($GLOBALS['_PEAR_Command_shortcuts'][$command])) {
            $command = $GLOBALS['_PEAR_Command_shortcuts'][$command];
        }
        if (isset($cmds[$command])) {
            $obj = &PEAR_Command::getObject($command);
            return $obj->getHelp($command);
        }
        return false;
    }
    // }}}
}

?>
