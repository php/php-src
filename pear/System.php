<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Tomas V.V.Cox <cox@idecnet.com>                             |
// +----------------------------------------------------------------------+
//
// $Id$
//

require_once 'PEAR.php';
require_once 'Console/Getopt.php';

$GLOBALS['_System_temp_files'] = array();

/**
* System offers cross plattform compatible system functions
*
* Static functions for different operations. Should work under
* Unix and Windows. The names and usage has been taken from its respectively
* GNU commands. The functions will return (bool) false on error and will
* trigger the error with the PHP trigger_error() function (you can silence
* the error by prefixing a '@' sign after the function call).
*
* Documentation on this class you can find in:
* http://pear.php.net/manual/
*
* Example usage:
* if (!@System::rm('-r file1 dir1')) {
*    print "could not delete file1 or dir1";
* }
*
* @package  System
* @author   Tomas V.V.Cox <cox@idecnet.com>
* @version  $Revision$
* @access   public
* @see      http://pear.php.net/manual/
*/
class System
{
    /**
    * returns the commandline arguments of a function
    *
    * @param    string  $argv           the commandline
    * @param    string  $short_options  the allowed option short-tags
    * @param    string  $long_options   the allowed option long-tags
    * @return   array   the given options and there values
    * @access private
    */
    function _parseArgs($argv, $short_options, $long_options = null)
    {
        if (!is_array($argv) && $argv !== null) {
            $argv = preg_split('/\s+/', $argv);
        }
        return Console_Getopt::getopt($argv, $short_options);
    }

    /**
    * Output errors with PHP trigger_error(). You can silence the errors
    * with prefixing a "@" sign to the function call: @System::mkdir(..);
    *
    * @param mixed $error a PEAR error or a string with the error message
    * @return bool false
    * @access private
    */
    function raiseError($error)
    {
        if (PEAR::isError($error)) {
            $error = $error->getMessage();
        }
        trigger_error($error, E_USER_WARNING);
        return false;
    }

    /**
    * Creates a nested array representing the structure of a directory
    *
    * System::_dirToStruct('dir1', 0) =>
    *   Array
    *    (
    *    [dirs] => Array
    *        (
    *            [0] => dir1
    *        )
    *
    *    [files] => Array
    *        (
    *            [0] => dir1/file2
    *            [1] => dir1/file3
    *        )
    *    )
    * @param    string  $sPath      Name of the directory
    * @param    integer $maxinst    max. deep of the lookup
    * @param    integer $aktinst    starting deep of the lookup
    * @return   array   the structure of the dir
    * @access   private
    */

    function _dirToStruct($sPath, $maxinst, $aktinst = 0)
    {
        $struct = array('dirs' => array(), 'files' => array());
        if (($dir = @opendir($sPath)) === false) {
            System::raiseError("Could not open dir $sPath");
            return $struct; // XXX could not open error
        }
        $struct['dirs'][] = $sPath; // XXX don't add if '.' or '..' ?
        $list = array();
        while ($file = readdir($dir)) {
            if ($file != '.' && $file != '..') {
                $list[] = $file;
            }
        }
        closedir($dir);
        sort($list);
        foreach($list as $val) {
            $path = $sPath . DIRECTORY_SEPARATOR . $val;
            if (is_dir($path)) {
                if ($aktinst < $maxinst || $maxinst == 0) {
                    $tmp = System::_dirToStruct($path, $maxinst, $aktinst+1);
                    $struct = array_merge_recursive($tmp, $struct);
                }
            } else {
                $struct['files'][] = $path;
            }
        }
        return $struct;
    }

    /**
    * Creates a nested array representing the structure of a directory and files
    *
    * @param    array $files Array listing files and dirs
    * @return   array
    * @see System::_dirToStruct()
    */
    function _multipleToStruct($files)
    {
        $struct = array('dirs' => array(), 'files' => array());
        settype($files, 'array');
        foreach ($files as $file) {
            if (is_dir($file)) {
                $tmp = System::_dirToStruct($file, 0);
                $struct = array_merge_recursive($tmp, $struct);
            } else {
                $struct['files'][] = $file;
            }
        }
        return $struct;
    }

    /**
    * The rm command for removing files.
    * Supports multiple files and dirs and also recursive deletes
    *
    * @param    string  $args   the arguments for rm
    * @return   mixed   PEAR_Error or true for success
    * @access   public
    */
    function rm($args)
    {
        $opts = System::_parseArgs($args, 'rf'); // "f" do nothing but like it :-)
        if (PEAR::isError($opts)) {
            return System::raiseError($opts);
        }
        foreach($opts[0] as $opt) {
            if ($opt[0] == 'r') {
                $do_recursive = true;
            }
        }
        $ret = true;
        if (isset($do_recursive)) {
            $struct = System::_multipleToStruct($opts[1]);
            foreach($struct['files'] as $file) {
                if (!@unlink($file)) {
                    $ret = false;
                }
            }
            foreach($struct['dirs'] as $dir) {
                if (!@rmdir($dir)) {
                    $ret = false;
                }
            }
        } else {
            foreach ($opts[1] as $file) {
                $delete = (is_dir($file)) ? 'rmdir' : 'unlink';
                if (!@$delete($file)) {
                    $ret = false;
                }
            }
        }
        return $ret;
    }

    /**
    * Make directories. Note that we use call_user_func('mkdir') to avoid
    * a problem with ZE2 calling System::mkDir instead of the native PHP func.
    *
    * @param    string  $args    the name of the director(y|ies) to create
    * @return   bool    True for success
    * @access   public
    */
    function mkDir($args)
    {
        $opts = System::_parseArgs($args, 'pm:');
        if (PEAR::isError($opts)) {
            return System::raiseError($opts);
        }
        $mode = 0777; // default mode
        foreach($opts[0] as $opt) {
            if ($opt[0] == 'p') {
                $create_parents = true;
            } elseif($opt[0] == 'm') {
                $mode = $opt[1];
            }
        }
        $ret = true;
        if (isset($create_parents)) {
            foreach($opts[1] as $dir) {
                $dirstack = array();
                while (!@is_dir($dir) && $dir != DIRECTORY_SEPARATOR) {
                    array_unshift($dirstack, $dir);
                    $dir = dirname($dir);
                }
                while ($newdir = array_shift($dirstack)) {
                    if (!call_user_func('mkdir', $newdir, $mode)) {
                        $ret = false;
                    }
                }
            }
        } else {
            foreach($opts[1] as $dir) {
                if (!@is_dir($dir) && !call_user_func('mkdir', $dir, $mode)) {
                    $ret = false;
                }
            }
        }
        return $ret;
    }

    /**
    * Concatenate files
    *
    * Usage:
    * 1) $var = System::cat('sample.txt test.txt');
    * 2) System::cat('sample.txt test.txt > final.txt');
    * 3) System::cat('sample.txt test.txt >> final.txt');
    *
    * Note: as the class use fopen, urls should work also (test that)
    *
    * @param    string  $args   the arguments
    * @return   boolean true on success
    * @access   public
    */
    function &cat($args)
    {
        $ret = null;
        $files = array();
        if (!is_array($args)) {
            $args = preg_split('/\s+/', $args);
        }
        for($i=0; $i < count($args); $i++) {
            if ($args[$i] == '>') {
                $mode = 'wb';
                $outputfile = $args[$i+1];
                break;
            } elseif ($args[$i] == '>>') {
                $mode = 'ab+';
                $outputfile = $args[$i+1];
                break;
            } else {
                $files[] = $args[$i];
            }
        }
        if (isset($mode)) {
            if (!$outputfd = fopen($outputfile, $mode)) {
                return System::raiseError("Could not open $outputfile");
            }
            $ret = true;
        }
        foreach ($files as $file) {
            if (!$fd = fopen($file, 'r')) {
                System::raiseError("Could not open $file");
                continue;
            }
            while ($cont = fread($fd, 2048)) {
                if (isset($outputfd)) {
                    fwrite($outputfd, $cont);
                } else {
                    $ret .= $cont;
                }
            }
            fclose($fd);
        }
        if (@is_resource($outputfd)) {
            fclose($outputfd);
        }
        return $ret;
    }

    /**
    * Creates temporary files or directories. This function will remove
    * the created files when the scripts finish its execution.
    *
    * Usage:
    *   1) $tempfile = System::mktemp("prefix");
    *   2) $tempdir  = System::mktemp("-d prefix");
    *   3) $tempfile = System::mktemp();
    *   4) $tempfile = System::mktemp("-t /var/tmp prefix");
    *
    * prefix -> The string that will be prepended to the temp name
    *           (defaults to "tmp").
    * -d     -> A temporary dir will be created instead of a file.
    * -t     -> The target dir where the temporary (file|dir) will be created. If
    *           this param is missing by default the env vars TMP on Windows or
    *           TMPDIR in Unix will be used. If these vars are also missing
    *           c:\windows\temp or /tmp will be used.
    *
    * @param   string  $args  The arguments
    * @return  mixed   the full path of the created (file|dir) or false
    * @see System::tmpdir()
    * @access  public
    */
    function mktemp($args = null)
    {
        static $first_time = true;
        $opts = System::_parseArgs($args, 't:d');
        if (PEAR::isError($opts)) {
            return System::raiseError($opts);
        }
        foreach($opts[0] as $opt) {
            if($opt[0] == 'd') {
                $tmp_is_dir = true;
            } elseif($opt[0] == 't') {
                $tmpdir = $opt[1];
            }
        }
        $prefix = (isset($opts[1][0])) ? $opts[1][0] : 'tmp';
        if (!isset($tmpdir)) {
            $tmpdir = System::tmpdir();
        }
        if (!System::mkDir("-p $tmpdir")) {
            return false;
        }
        $tmp = tempnam($tmpdir, $prefix);
        if (isset($tmp_is_dir)) {
            unlink($tmp); // be careful possible race condition here
            if (!call_user_func('mkdir', $tmp, 0700)) {
                return System::raiseError("Unable to create temporary directory $tmpdir");
            }
        }
        $GLOBALS['_System_temp_files'][] = $tmp;
        if ($first_time) {
            PEAR::registerShutdownFunc(array('System', '_removeTmpFiles'));
            $first_time = false;
        }
        return $tmp;
    }

    /**
    * Remove temporary files created my mkTemp. This function is executed
    * at script shutdown time
    *
    * @access private
    */
    function _removeTmpFiles()
    {
        if (count($GLOBALS['_System_temp_files'])) {
            $delete = $GLOBALS['_System_temp_files'];
            array_unshift($delete, '-r');
            System::rm($delete);
        }
    }

    /**
    * Get the path of the temporal directory set in the system
    * by looking in its environments variables.
    *
    * @return string The temporal directory on the system
    */
    function tmpdir()
    {
        if (OS_WINDOWS) {
            if (($var=$_ENV['TEMP']) || $var=getenv('TEMP')) {
                 return $var;
            }
            if (($var=$_ENV['TMP']) || $var=getenv('TMP')) {
                 return $var;
            }
            if (($var=$_ENV['windir']) || $var=getenv('windir')) {
                 return $var;
            }
            return getenv('SystemRoot') . '\temp';
        }
        if (($var=$_ENV['TMPDIR']) || $var=getenv('TMPDIR')) {
             return $var;
        }
        return '/tmp';
    }

    /**
    * (pajoye) Removed, problem with php.ini-recommanded, E removed
    *
    * (cox) I always get $_ENV empty in both Windows and Linux
    * with all PHP version <= 4.2.1

    function _myenv($var)
    {
        if (!empty($_ENV)) {
            return isset($_ENV[$var]) ? $_ENV[$var] : false;
        }
        return getenv($var);
    }
    */

    /**
    * The "type" command (show the full path of a command)
    *
    * @param string $program The command to search for
    * @return mixed A string with the full path or false if not found
    * @author Stig Bakken <ssb@fast.no>
    */
    function type($program)
    {
        // full path given
        if (basename($program) != $program) {
            return (@is_executable($program)) ? $program : false;
        }
        // XXX FIXME honor safe mode
        $path_delim = OS_WINDOWS ? ';' : ':';
        $exe_suffix = OS_WINDOWS ? '.exe' : '';
        $path_elements = explode($path_delim, getenv('PATH'));
        foreach ($path_elements as $dir) {
            $file = $dir . DIRECTORY_SEPARATOR . $program . $exe_suffix;
            if (@is_file($file) && @is_executable($file)) {
                return $file;
            }
        }
        return false;
    }
}
?>