<?php
/**
 * File/Directory manipulation
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
 * @package    System
 * @author     Tomas V.V.Cox <cox@idecnet.com>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * base class
 */
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
* In case you need to to pass file names with spaces,
* pass the params as an array:
*
* System::rm(array('-r', $file1, $dir1));
*
* @category   pear
* @package    System
* @author     Tomas V.V. Cox <cox@idecnet.com>
* @copyright  1997-2005 The PHP Group
* @license    http://www.php.net/license/3_0.txt  PHP License 3.0
* @version    Release: @package_version@
* @link       http://pear.php.net/package/PEAR
* @since      Class available since Release 0.1
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
            $argv = preg_split('/\s+/', $argv, -1, PREG_SPLIT_NO_EMPTY);
        }
        return Console_Getopt::getopt2($argv, $short_options);
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
        $struct['dirs'][] = $sPath = realpath($sPath); // XXX don't add if '.' or '..' ?
        $list = array();
        while (false !== ($file = readdir($dir))) {
            if ($file != '.' && $file != '..') {
                $list[] = $file;
            }
        }
        closedir($dir);
        sort($list);
        if ($aktinst < $maxinst || $maxinst == 0) {
            foreach($list as $val) {
                $path = $sPath . DIRECTORY_SEPARATOR . $val;
                if (is_dir($path) && !is_link($path)) {
                    $tmp = System::_dirToStruct($path, $maxinst, $aktinst+1);
                    $struct = array_merge_recursive($tmp, $struct);
                } else {
                    $struct['files'][] = $path;
                }
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
            if (is_dir($file) && !is_link($file)) {
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
    * Make directories.
    *
    * The -p option will create parent directories
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
                // if the mode is clearly an octal number (starts with 0)
                // convert it to decimal
                if (strlen($opt[1]) && $opt[1]{0} == '0') {
                    $opt[1] = octdec($opt[1]);
                } else {
                    // convert to int
                    $opt[1] += 0;
                }
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
                    if (!is_writeable(dirname($newdir))) {
                        $ret = false;
                        break;
                    }
                    if (!mkdir($newdir, $mode)) {
                        $ret = false;
                    }
                }
            }
        } else {
            foreach($opts[1] as $dir) {
                if (!@is_dir($dir) && !mkdir($dir, $mode)) {
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
            $args = preg_split('/\s+/', $args, -1, PREG_SPLIT_NO_EMPTY);
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
                $err = System::raiseError("Could not open $outputfile");
                return $err;
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
        if (!System::mkDir(array('-p', $tmpdir))) {
            return false;
        }
        $tmp = tempnam($tmpdir, $prefix);
        if (isset($tmp_is_dir)) {
            unlink($tmp); // be careful possible race condition here
            if (!mkdir($tmp, 0700)) {
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
            $GLOBALS['_System_temp_files'] = array();
        }
    }

    /**
    * Get the path of the temporal directory set in the system
    * by looking in its environments variables.
    * Note: php.ini-recommended removes the "E" from the variables_order setting,
    * making unavaible the $_ENV array, that s why we do tests with _ENV
    *
    * @return string The temporal directory on the system
    */
    function tmpdir()
    {
        if (OS_WINDOWS) {
            if ($var = isset($_ENV['TEMP']) ? $_ENV['TEMP'] : getenv('TEMP')) {
                return $var;
            }
            if ($var = isset($_ENV['TMP']) ? $_ENV['TMP'] : getenv('TMP')) {
                return $var;
            }
            if ($var = isset($_ENV['windir']) ? $_ENV['windir'] : getenv('windir')) {
                return $var;
            }
            return getenv('SystemRoot') . '\temp';
        }
        if ($var = isset($_ENV['TMPDIR']) ? $_ENV['TMPDIR'] : getenv('TMPDIR')) {
            return $var;
        }
        return '/tmp';
    }

    /**
    * The "which" command (show the full path of a command)
    *
    * @param string $program The command to search for
    * @param mixed  $fallback Value to return if $program is not found
    *
    * @return mixed A string with the full path or false if not found
    * @author Stig Bakken <ssb@php.net>
    */
    function which($program, $fallback = false)
    {
        // enforce API
        if (!is_string($program) || '' == $program) {
            return $fallback;
        }

        // available since 4.3.0RC2
        if (defined('PATH_SEPARATOR')) {
            $path_delim = PATH_SEPARATOR;
        } else {
            $path_delim = OS_WINDOWS ? ';' : ':';
        }
        // full path given
        if (basename($program) != $program) {
            $path_elements[] = dirname($program);
            $program = basename($program);
        } else {
            // Honor safe mode
            if (!ini_get('safe_mode') || !$path = ini_get('safe_mode_exec_dir')) {
                $path = getenv('PATH');
                if (!$path) {
                    $path = getenv('Path'); // some OSes are just stupid enough to do this
                }
            }
            $path_elements = explode($path_delim, $path);
        }

        if (OS_WINDOWS) {
            $exe_suffixes = getenv('PATHEXT')
                                ? explode($path_delim, getenv('PATHEXT'))
                                : array('.exe','.bat','.cmd','.com');
            // allow passing a command.exe param
            if (strpos($program, '.') !== false) {
                array_unshift($exe_suffixes, '');
            }
            // is_executable() is not available on windows for PHP4
            $pear_is_executable = (function_exists('is_executable')) ? 'is_executable' : 'is_file';
        } else {
            $exe_suffixes = array('');
            $pear_is_executable = 'is_executable';
        }

        foreach ($exe_suffixes as $suff) {
            foreach ($path_elements as $dir) {
                $file = $dir . DIRECTORY_SEPARATOR . $program . $suff;
                if ($pear_is_executable($file)) {
                    return $file;
                }
            }
        }
        return $fallback;
    }

    /**
    * The "find" command
    *
    * Usage:
    *
    * System::find($dir);
    * System::find("$dir -type d");
    * System::find("$dir -type f");
    * System::find("$dir -name *.php");
    * System::find("$dir -name *.php -name *.htm*");
    * System::find("$dir -maxdepth 1");
    *
    * Params implmented:
    * $dir            -> Start the search at this directory
    * -type d         -> return only directories
    * -type f         -> return only files
    * -maxdepth <n>   -> max depth of recursion
    * -name <pattern> -> search pattern (bash style). Multiple -name param allowed
    *
    * @param  mixed Either array or string with the command line
    * @return array Array of found files
    *
    */
    function find($args)
    {
        if (!is_array($args)) {
            $args = preg_split('/\s+/', $args, -1, PREG_SPLIT_NO_EMPTY);
        }
        $dir = array_shift($args);
        $patterns = array();
        $depth = 0;
        $do_files = $do_dirs = true;
        for ($i = 0; $i < count($args); $i++) {
            switch ($args[$i]) {
                case '-type':
                    if (in_array($args[$i+1], array('d', 'f'))) {
                        if ($args[$i+1] == 'd') {
                            $do_files = false;
                        } else {
                            $do_dirs = false;
                        }
                    }
                    $i++;
                    break;
                case '-name':
                    if (OS_WINDOWS) {
                        if ($args[$i+1]{0} == '\\') {
                            // prepend drive
                            $args[$i+1] = addslashes(substr(getcwd(), 0, 2) . $args[$i + 1]);
                        }
                        // escape path separators to avoid PCRE problems
                        $args[$i+1] = str_replace('\\', '\\\\', $args[$i+1]);
                    }
                    $patterns[] = "(" . preg_replace(array('/\./', '/\*/'),
                                                     array('\.', '.*', ),
                                                     $args[$i+1])
                                      . ")";
                    $i++;
                    break;
                case '-maxdepth':
                    $depth = $args[$i+1];
                    break;
            }
        }
        $path = System::_dirToStruct($dir, $depth);
        if ($do_files && $do_dirs) {
            $files = array_merge($path['files'], $path['dirs']);
        } elseif ($do_dirs) {
            $files = $path['dirs'];
        } else {
            $files = $path['files'];
        }
        if (count($patterns)) {
            $patterns = implode('|', $patterns);
            $ret = array();
            for ($i = 0; $i < count($files); $i++) {
                if (preg_match("#^$patterns\$#", $files[$i])) {
                    $ret[] = $files[$i];
                }
            }
            return $ret;
        }
        return $files;
    }
}
?>
