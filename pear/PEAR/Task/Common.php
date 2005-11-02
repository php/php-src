<?php
/**
 * PEAR_Task_Common, base class for installer tasks
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
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.0a1
 */
/**#@+
 * Error codes for task validation routines
 */
define('PEAR_TASK_ERROR_NOATTRIBS', 1);
define('PEAR_TASK_ERROR_MISSING_ATTRIB', 2);
define('PEAR_TASK_ERROR_WRONG_ATTRIB_VALUE', 3);
define('PEAR_TASK_ERROR_INVALID', 4);
/**#@-*/
define('PEAR_TASK_PACKAGE', 1);
define('PEAR_TASK_INSTALL', 2);
define('PEAR_TASK_PACKAGEANDINSTALL', 3);
/**
 * A task is an operation that manipulates the contents of a file.
 *
 * Simple tasks operate on 1 file.  Multiple tasks are executed after all files have been
 * processed and installed, and are designed to operate on all files containing the task.
 * The Post-install script task simply takes advantage of the fact that it will be run
 * after installation, replace is a simple task.
 *
 * Combining tasks is possible, but ordering is significant.
 *
 * <file name="test.php" role="php">
 *  <tasks:replace from="@data-dir@" to="data_dir" type="pear-config"/>
 *  <tasks:postinstallscript/>
 * </file>
 *
 * This will first replace any instance of @data-dir@ in the test.php file
 * with the path to the current data directory.  Then, it will include the
 * test.php file and run the script it contains to configure the package post-installation.
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 * @abstract
 */
class PEAR_Task_Common
{
    /**
     * Valid types for this version are 'simple' and 'multiple'
     *
     * - simple tasks operate on the contents of a file and write out changes to disk
     * - multiple tasks operate on the contents of many files and write out the
     *   changes directly to disk
     *
     * Child task classes must override this property.
     * @access protected
     */
    var $type = 'simple';
    /**
     * Determines which install phase this task is executed under
     */
    var $phase = PEAR_TASK_INSTALL;
    /**
     * @access protected
     */
    var $config;
    /**
     * @access protected
     */
    var $registry;
    /**
     * @access protected
     */
    var $logger;
    /**
     * @access protected
     */
    var $installphase;
    /**
     * @param PEAR_Config
     * @param PEAR_Common
     */
    function PEAR_Task_Common(&$config, &$logger, $phase)
    {
        $this->config = &$config;
        $this->registry = &$config->getRegistry();
        $this->logger = &$logger;
        $this->installphase = $phase;
        if ($this->type == 'multiple') {
            $GLOBALS['_PEAR_TASK_POSTINSTANCES'][get_class($this)][] = &$this;
        }
    }

    /**
     * Validate the basic contents of a task tag.
     * @param PEAR_PackageFile_v2
     * @param array
     * @param PEAR_Config
     * @param array the entire parsed <file> tag
     * @return true|array On error, return an array in format:
     *    array(PEAR_TASK_ERROR_???[, param1][, param2][, ...])
     *
     *    For PEAR_TASK_ERROR_MISSING_ATTRIB, pass the attribute name in
     *    For PEAR_TASK_ERROR_WRONG_ATTRIB_VALUE, pass the attribute name and an array
     *    of legal values in
     * @static
     * @abstract
     */
    function validXml($pkg, $xml, &$config, $fileXml)
    {
    }

    /**
     * Initialize a task instance with the parameters
     * @param array raw, parsed xml
     * @param array attributes from the <file> tag containing this task
     * @param string|null last installed version of this package
     * @abstract
     */
    function init($xml, $fileAttributes, $lastVersion)
    {
    }

    /**
     * Begin a task processing session.  All multiple tasks will be processed after each file
     * has been successfully installed, all simple tasks should perform their task here and
     * return any errors using the custom throwError() method to allow forward compatibility
     *
     * This method MUST NOT write out any changes to disk
     * @param PEAR_PackageFile_v2
     * @param string file contents
     * @param string the eventual final file location (informational only)
     * @return string|false|PEAR_Error false to skip this file, PEAR_Error to fail
     *         (use $this->throwError), otherwise return the new contents
     * @abstract
     */
    function startSession($pkg, $contents, $dest)
    {
    }

    /**
     * This method is used to process each of the tasks for a particular multiple class
     * type.  Simple tasks need not implement this method.
     * @param array an array of tasks
     * @access protected
     * @static
     * @abstract
     */
    function run($tasks)
    {
    }

    /**
     * @static
     * @final
     */
    function hasPostinstallTasks()
    {
        return isset($GLOBALS['_PEAR_TASK_POSTINSTANCES']);
    }

    /**
     * @static
     * @final
     */
     function runPostinstallTasks()
     {
         foreach ($GLOBALS['_PEAR_TASK_POSTINSTANCES'] as $class => $tasks) {
             $err = call_user_func(array($class, 'run'),
                  $GLOBALS['_PEAR_TASK_POSTINSTANCES'][$class]);
             if ($err) {
                 return PEAR_Task_Common::throwError($err);
             }
         }
         unset($GLOBALS['_PEAR_TASK_POSTINSTANCES']);
    }

    /**
     * Determines whether a role is a script
     * @return bool
     */
    function isScript()
    {
        return $this->type == 'script';
    }

    function throwError($msg, $code = -1)
    {
        include_once 'PEAR.php';
        return PEAR::raiseError($msg, $code);
    }
}
?>