<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//

require_once "PEAR.php";

register_shutdown_function("__PEAR_Installer_cleanup");

/**
 * Administration class used to install PEAR packages and maintain the
 * class definition cache.
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR_Installer extends PEAR
{
    // {{{ properties

    /** stack of elements, gives some sort of XML context */
    var $element_stack;

    /** name of currently parsed XML element */
    var $current_element;

    /** array of attributes of the currently parsed XML element */
    var $current_attributes = array();

    /** assoc with information about the package */
    var $pkginfo = array();

    /** name of the package directory, for example Foo-1.0 */
    var $pkgdir;

    /** directory where PHP code files go */
    var $pear_phpdir = PEAR_INSTALL_DIR;

    /** directory where PHP extension files go */
    var $pear_extdir = PEAR_EXTENSION_DIR;

    /** directory where documentation goes */
    var $pear_docdir = '';

    /** directory where the package wants to put files, relative
     *  to one of the three previous dirs
     */
    var $destdir = '';

    /** debug mode (boolean) */
    var $debug = false;

    /** class loading cache */
    var $cache = array();

    /** temporary directory */
    var $tmpdir;

    /** file pointer for cache file if open */
    var $cache_fp;

    // }}}

    // {{{ constructor

    function PEAR_Installer() {
	$this->PEAR();
	$this->cacheLoad("$this->pear_phpdir/.cache");
    }

    // }}}
    // {{{ destructor

    function _PEAR_Installer() {
	$this->_PEAR();
	if ($this->tmpdir && is_dir($this->tmpdir)) {
	    system("rm -rf $this->tmpdir");
	}
	if ($this->cache_fp && is_resource($this->cache_fp)) {
	    flock($this->cache_fp, LOCK_UN);
	    fclose($this->cache_fp);
	}
	$this->tmpdir = null;
	$this->cache_fp = null;
    }

    // }}}

    // {{{ raiseError()

    function raiseError($msg)
    {
        return new PEAR_Error("$msg\n", 0, PEAR_ERROR_DIE);
    }

    // }}}
    // {{{ mkDirHier()

    function mkDirHier($dir)
    {
        $dirstack = array();
        // XXX FIXME this does not work on Windows!
        while (!is_dir($dir) && $dir != "/") {
            array_unshift($dirstack, $dir);
            $dir = dirname($dir);
        }
        while ($newdir = array_shift($dirstack)) {
            if (mkdir($newdir, 0777)) {
                $this->log(1, "created dir $newdir");
            } else {
                return $this->raiseError("mkdir($newdir) failed");
            }
        }
    }

    // }}}
    // {{{ log()

    function log($level, $msg)
    {
        if ($this->debug >= $level) {
            print "$msg\n";
        }
    }

    // }}}

    // {{{ cacheLock()

    function cacheLock() {
	$fp = $this->cache_fp;
	if (!is_resource($fp)) {
	    $this->cache_fp = $fp = fopen($this->cache_file, "r");
	}
	return flock($fp, LOCK_EX);
    }

    // }}}
    // {{{ cacheUnlock()

    function cacheUnlock() {
	$fp = $this->cache_fp;
	if (!is_resource($fp)) {
	    $this->cache_fp = $fp = fopen($this->cache_file, "r");
	    $doclose = true;
	}
	$ret = flock($fp, LOCK_EX);
	if ($doclose) {
	    fclose($fp);
	}
	return $ret;
    }

    // }}}
    // {{{ cacheLoad()

    function cacheLoad($file) {
	$this->cache_file = $file;
	if (!file_exists($file)) {
	    touch($file);
	}
	$fp = $this->cache_fp = fopen($file, "r");
	$this->cacheLock();
	while ($line = fgets($fp, 2048)) {
	    list($type, $name, $file) = explode(" ", trim($line));
	    $this->cache[$type][$name] = $file;
	}
    }

    // }}}
    // {{{ cacheSave()

    function cacheSave() {
	$fp = $this->cache_fp;
	$wfp = fopen($this->cache_file, "w");
	if (!$wfp) {
	    return false;
	}
	if (is_resource($fp)) {
	    fclose($fp);
	}
	$this->cache_fp = $fp = $wfp;
	reset($this->cache);
	while (list($type, $entry) = each($this->cache)) {
	    reset($entry);
	    while (list($name, $file) = each($entry)) {
		fwrite($fp, "$type $name $file\n");
	    }
	}
	fclose($fp);
	$this->cache_fp = $fp = null;
    }

    // }}}
    // {{{ cacheUpdateFrom()

    function cacheUpdateFrom($file) {
        /*
	$new = $this->classesDeclaredBy($file);
	reset($new);
	while (list($i, $name) = each($new)) {
	    $this->cache['class'][$name] = $file;
	}
        */
    }

    // }}}

    // {{{ install()

    /**
     * Installs the files within the package file specified.
     *
     * @param $pkgfile path to the package file
     *
     * @return bool true if successful, false if not
     */
    function install($pkgfile) {
        global $_PEAR_Installer_tempfiles;
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            $need_download = true;
        } elseif (!file_exists($pkgfile)) {
	    return $this->raiseError("$pkgfile: no such file");
	}

        if ($need_download) {
            $file = basename($pkgfile);
            $downloaddir = "/tmp/pearinstall";
            $this->mkDirHier($downloaddir);
            $downloadfile = "$downloaddir/$file";
            $this->log(1, "downloading $pkgfile...");
            $fp = @fopen($pkgfile, "r");
            if (!$fp) {
                return $this->raiseError("$pkgfile: failed to download ($php_errormsg)");
            }
            $wp = @fopen($downloadfile, "w");
            if (!$wp) {
                return $this->raiseError("$downloadfile: write failed ($php_errormsg)");
            }
            $bytes = 0;
            while ($data = @fread($fp, 16384)) {
                $bytes += strlen($data);
                if (!@fwrite($wp, $data)) {
                    return $this->raiseError("$downloadfile: write failed ($php_errormsg)");
                }
            }
            $pkgfile = $downloadfile;
            fclose($fp);
            fclose($wp);
            $this->log(1, "...done, $bytes bytes");
            $_PEAR_Installer_tempfiles[] = $downloadfile;
        }
	$fp = popen("gzip -dc $pkgfile | tar -tf -", "r");
	if (!$fp) {
	    return $this->raiseError("Unable to examine $pkgfile (gzip or tar failed)");
	}
	while ($line = fgets($fp, 4096)) {
	    $line = rtrim($line);
	    if (preg_match('!^[^/]+/package.xml$!', $line)) {
		if ($descfile) {
		    return $this->raiseError("Invalid package: multiple package.xml files at depth one!");
		}
		$descfile = $line;
	    }
	}
	pclose($fp);

	if (!$descfile) {
	    return $this->raiseError("Invalid package: no package.xml file found!");
	}

	$this->tmpdir = tempnam("/tmp", "pear");
        unlink($this->tmpdir);
	if (!mkdir($this->tmpdir, 0755)) {
	    return $this->raiseError("Unable to create temporary directory $this->tmpdir.");
	}
        $_PEAR_Installer_tempfiles[] = $this->tmpdir;
	$pwd = trim(`pwd`);

	if (substr($pkgfile, 0, 1) == "/") {
	    $pkgfilepath = $pkgfile;
	} else {
	    $pkgfilepath = $pwd.'/'.$pkgfile;
	}

	if (!chdir($this->tmpdir)) {
	    return $this->raiseError("Unable to chdir to $this->tmpdir.");
	}

	system("gzip -dc $pkgfilepath | tar -xf -");

	if (!file_exists($descfile)) {
	    return $this->raiseError("Huh?  No package.xml file after extracting the archive.");
	}

	$this->pkgdir = dirname($descfile);

	$fp = fopen($descfile, "r");
	$xp = xml_parser_create();
	if (!$xp) {
	    return $this->raiseError("Unable to create XML parser.");
	}
	xml_set_object($xp, &$this);
	xml_set_element_handler($xp, "startHandler", "endHandler");
	xml_set_character_data_handler($xp, "charHandler");
	xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, false);

	$this->element_stack = array();
	$this->pkginfo = array();
	$this->current_element = false;
	$destdir = '';

	while ($data = fread($fp, 2048)) {
	    if (!xml_parse($xp, $data, feof($fp))) {
		$err = $this->raiseError(sprintf("XML error: %s at line %d",
							xml_error_string(xml_get_error_code($xp)),
							xml_get_current_line_number($xp)));
		xml_parser_free($xp);
		return $err;
	    }
	}

	xml_parser_free($xp);

	return true;
    }

    // }}}
    // {{{ startHandler()

    function startHandler($xp, $name, $attribs) {
	array_push($this->element_stack, $name);
	$this->current_element = $name;
	$this->current_attributes = $attribs;
	switch ($name) {
	    case "Package":
		if (strtolower($attribs["Type"]) != "binary") {
		    return $this->raiseError("Invalid package: only binary packages supported yet.");
		}
		$this->pkginfo['pkgtype'] = strtolower($attribs["Type"]);
		break;
	}
    }

    // }}}
    // {{{ endHandler()

    function endHandler($xp, $name) {
	array_pop($this->element_stack);
	$this->current_element = $this->element_stack[sizeof($this->element_stack)-1];
    }

    // }}}
    // {{{ charHandler()

    function charHandler($xp, $data) {
        // XXX FIXME: $data may be incomplete, all of this code should
        // actually be in endHandler.
        //
	switch ($this->current_element) {
	    case "Dir":
		if (!$this->pear_phpdir) {
		    break;
		}
                $type = $this->current_attributes["Type"];
		$dir = trim($data);
		$d = "$this->pear_phpdir/$this->destdir/$dir";
		if (substr($dir, 0, 1) == "/") {
		    $this->destdir = substr($dir, 1);
		} else {
                    $this->destdir = $dir;
                }
		break;
		if (is_file($d)) {
                    return $this->raiseError("mkdir $d failed: is a file");
		}
		if (is_dir($d)) {
		    break;
		}
		if (!mkdir($d, 0755)) {
                    return $this->raiseError("mkdir $d failed");
		    break;
		}
                $this->log(1, "created dir $d");
		break;
	    case "File":
		if (!$this->pear_phpdir) {
		    break;
		}
                $type = strtolower($this->current_attributes["Role"]);
		$file = trim($data);
                $updatecache = false;
                switch ($type) {
                    case "test":
                        $d = ""; // don't install test files for now
                        break;
                    default:
                        if ($this->destdir) {
                            $d = "$this->pear_phpdir/$this->destdir";
                        } else {
                            $d = $this->pear_phpdir;
                        }
                        $updatecache = true;
                        break;
                }
                if (!$d) {
                    break;
                }
                if (!is_dir($d)) {
                    $this->mkDirHier($d);
                }
                $bfile = basename($file);
		if (!copy("$this->pkgdir/$file", "$d/$bfile")) {
		    $this->log(0, "failed to copy $this->pkgdir/$file to $d");
		    break;
		}
                if ($updatecache) {
                    $this->cacheUpdateFrom("$d/$file");
                }
                $this->log(1, "installed $d/$bfile");
		break;
	}
    }

    // }}}

    // {{{ classesDeclaredBy()

    /**
     * Find out which new classes are defined by a file.
     *
     * @param $file file name passed to "include"
     *
     * @return array classes that were defined
     */
    function classesDeclaredBy($file) {
	$before = get_declared_classes();
        ob_start();
	include($file);
        ob_end_clean();
	$after = get_declared_classes();
	// using array_slice to renumber array
	$diff = array_slice(array_diff($after, $before), 0);
	return $diff;
    }

// }}}

    // {{{ lockDir()

    /**
     * Uses advisory locking (flock) to temporarily claim $dir as its
     * own.
     *
     * @param $dir the directory to lock
     *
     * @return bool true if successful, false if not
     */
    function lockDir($dir) {
	$lockfile = "$dir/.lock";
	if (!file_exists($lockfile)) {
	    if (!touch($lockfile)) {
		// could not create lockfile!
		return false;
	    }
	}
	$fp = fopen($lockfile, "r");
	if (!flock($fp, LOCK_EX)) {
	    // could not create lock!
	    return false;
	}
	return true;
    }

    // }}}
}

function __PEAR_Installer_cleanup()
{
    global $_PEAR_Installer_tempfiles;
    if (is_array($_PEAR_Installer_tempfiles)) {
        while ($file = array_shift($_PEAR_Installer_tempfiles)) {
            if (is_dir($file)) {
                system("rm -rf $file"); // XXX FIXME Windows
            } else {
                unlink($file);
            }
        }
    }
}

?>
