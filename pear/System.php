<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors:   Tomas V.V.Cox <cox@idecnet.com>                           |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$
//

// TODO:
// - Test under Windows (help is really appreciated in this point)
// - Build strong tests
// - Error reporting (now shows standar php errors)
// - Write doc

require_once 'PEAR.php';
require_once 'Console/Getopt.php';

/**
* System offers cross plattform compatible system functions
*
* Static functions for different operations. Should work under
* Unix and Windows. The names and usage has been taken from its repectively
* GNU commands.
*
* Usage: System::rm('-r file1 dir1');
*
* ------------------- EXPERIMENTAL STATUS -------------------
*
* @package  System
* @author   Tomas V.V.Cox <cox@idecnet.com>
* @version  $Version$
* @access   public
*/
class System extends PEAR
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
        if (!is_array($argv)) {
            $argv = preg_split('/\s+/', $argv);
        }
        $options = Console_Getopt::getopt($argv, $short_options);
        return $options;
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
        foreach($files as $file) {
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
            return $opts;
        }
        foreach($opts[0] as $opt) {
            if ($opt[0] == 'r') {
                $do_recursive = true;
            }
        }
        if (isset($do_recursive)) {
            $struct = System::_multipleToStruct($opts[1]);
            if (PEAR::isError($struct)) {
                return $struct;
            }
            foreach($struct['files'] as $file) {
                unlink($file); // XXXX Works under Windows?
            }
            foreach($struct['dirs'] as $dir) {
                rmdir($dir);
            }
        } else {
            foreach ($opts[1] as $file) {
                $delete = (is_dir($file)) ? 'rmdir' : 'unlink'; // XXXX Windows?
                $delete($file);
            }
        }
        return true;
    }

    /**
    * Make directories
    *
    * @param    string  $args    the name of the director(y|ies) to create
    * @return   mixed   PEAR_Error or true for success
    * @access   public    
    */
    function mkDir($args)
    {
        $opts = System::_parseArgs($args, 'pm:');
        if (PEAR::isError($opts)) {
            return $opts;
        }
        $mode = 0777; // default mode
        foreach($opts[0] as $opt) {
            if ($opt[0] == 'p') {
                $create_parents = true;
            } elseif($opt[0] == 'm') {
                $mode = $opt[1];
            }
        }
        if (isset($create_parents)) {
            foreach($opts[1] as $dir) {
                $dirstack = array();
                while (!@is_dir($dir) && $dir != DIRECTORY_SEPARATOR) {
                    array_unshift($dirstack, $dir);
                    $dir = dirname($dir);
                }
                while ($newdir = array_shift($dirstack)) {
                    if (!mkdir($newdir, $mode)) {
                        break; // XXX error
                    }
                }
            }
        } else {
            foreach($opts[1] as $dir) {
                if (!mkdir($dir, $mode)) {
                    continue; // XXX error
                }
            }
        }
        return true;
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
                return $this->raiseError("Could not open $outputfile");
            }
            $ret = true;
        }
        foreach($files as $file) {
            if (!$fd = fopen($file, 'r')) {
                return $this->raiseError("Could not open $file");
            }
            while(!feof($fd)) {
                $cont = fread($fd, 2048);
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

}
?>