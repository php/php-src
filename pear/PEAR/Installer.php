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

/**
 * Administration class used to install PEAR packages and maintain the
 * class definition cache.
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR_Installer extends PEAR {

    // {{{ properties

    /** stack of elements, gives some sort of XML context */
    var $element_stack;

    /** name of currently parsed XML element */
    var $current_element;

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
	$new = $this->classesDeclaredBy($file);
	reset($new);
	while (list($i, $name) = each($new)) {
	    $this->cache['class'][$name] = $file;
	}
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
	if (!file_exists($pkgfile)) {
	    return new PEAR_Installer_Error("No such file: $pkgfile");
	}

	$fp = popen("gzip -dc $pkgfile | tar -tf -", "r");
	if (!$fp) {
	    return new PEAR_Installer_Error("Unable to examine $pkgfile (gzip or tar failed)\n");
	}
	while ($line = fgets($fp, 4096)) {
	    $line = rtrim($line);
	    if (preg_match('!^[^/]+/package.xml$!', $line)) {
		if ($descfile) {
		    return new PEAR_Installer_Error("Invalid package: multiple package.xml files at depth one!\n");
		}
		$descfile = $line;
	    }
	}
	pclose($fp);

	if (!$descfile) {
	    return new PEAR_Installer_Error("Invalid package: no package.xml file found!\n");
	}

	$this->tmpdir = tempnam("/tmp", "pear");
	if (!mkdir($this->tmpdir, 0755)) {
	    return new PEAR_Installer_Error("Unable to create temporary directory $this->tmpdir.\n");
	}

	$pwd = trim(`pwd`);

	if (substr($pkgfile, 0, 1) == "/") {
	    $pkgfilepath = $pkgfile;
	} else {
	    $pkgfilepath = $pwd.'/'.$pkgfile;
	}

	if (!chdir($this->tmpdir)) {
	    return new PEAR_Installer_Error("Unable to chdir to $this->tmpdir.\n");
	}

	system("gzip -dc $pkgfilepath | tar -xf -");

	if (!file_exists($descfile)) {
	    return new PEAR_Installer_Error("Huh?  No package.xml file after extracting the archive.\n");
	}

	$this->pkgdir = dirname($descfile);

	$fp = fopen($descfile, "r");
	$xp = xml_parser_create();
	if (!$xp) {
	    return new PEAR_Installer_Error("Unable to create XML parser.\n");
	}
	xml_set_object($xp, &$this);
	xml_set_element_handler($xp, "start_handler", "end_handler");
	xml_set_character_data_handler($xp, "char_handler");
	xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, false);

	$this->element_stack = array();
	$this->pkginfo = array();
	$this->current_element = false;
	$destdir = '';

	while ($data = fread($fp, 2048)) {
	    if (!xml_parse($xp, $data, feof($fp))) {
		$err = new PEAR_Installer_Error(sprintf("XML error: %s at line %d",
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
    // {{{ start_handler()

    function start_handler($xp, $name, $attribs) {
	array_push($this->element_stack, $name);
	$this->current_element = $name;
	switch ($name) {
	    case "Package":
		if (strtolower($attribs["Type"]) != "binary") {
		    return new PEAR_Installer_Error("Invalid package: only binary packages supported yet.\n");
		}
		$this->pkginfo['pkgtype'] = strtolower($attribs["Type"]);
		break;
	}
    }

    // }}}
    // {{{ end_handler()

    function end_handler($xp, $name) {
	array_pop($this->element_stack);
	$this->current_element = $this->element_stack[sizeof($this->element_stack)-1];
    }

    // }}}
    // {{{ char_handler()

    function char_handler($xp, $data) {
	switch ($this->current_element) {
	    case "DestDir":
		$this->destdir = trim($data);
		if (substr($this->destdir, 0, 1) == "/") {
		    $this->destdir = substr($this->destdir, 1);
		}
		break;
	    case "Dir":
		if (!$this->pear_phpdir) {
		    break;
		}
		$dir = trim($data);
		$d = "$this->pear_phpdir/$this->destdir/$dir";
		if (is_file($d)) {
		    print "Error: wanted to create the directory $d\n";
		    print "       but it was already a file.\n";
		    break;
		}
		if (is_dir($d)) {
		    break;
		}
		if (!mkdir($d, 0755)) {
		    print "Error: could not mkdir $d\n";
		    break;
		}
		if ($this->debug) print "[debug] created directory $d\n";
		break;
	    case "File":
		if (!$this->pear_phpdir) {
		    break;
		}
		$file = trim($data);
		$d = "$this->pear_phpdir/$this->destdir";
		if (!copy("$this->pkgdir/$file", "$d/$file")) {
		    print "Error: failed to copy $this->pkgdir/$file to $d\n";
		    break;
		}
		$this->cacheUpdateFrom("$d/$file");
		if ($this->debug) print "[debug] installed $d/$file\n";
		break;
	    case "ExtDir":
		if (!$this->pear_extdir) {
		    break;
		}
		$dir = trim($data);
		$d = "$this->pear_extdir/$this->destdir/$dir";
		if (is_file($d)) {
		    print "Error: wanted to create the directory $d\n";
		    print "       but it was already a file.\n";
		    break;
		}
		if (is_dir($d)) {
		    continue 2;
		}
		if (!mkdir($d, 0755)) {
		    print "Error: could not mkdir $d\n";
		    break;
		}
		if ($this->debug) print "[debug] created directory $d\n";
		break;
	    case "ExtFile":
		if (!$this->pear_extdir) {
		    break;
		}
		$file = trim($data);
		$d = "$this->pear_extdir/$this->destdir";
		if (!copy("$this->pkgdir/$file", "$d/$file")) {
		    print "Error: failed to copy $this->pkgdir/$file to $d\n";
		    break;
		}
		if ($this->debug) print "[debug] installed $d/$file\n";
		break;
	    case "DocDir":
		if (!$this->pear_docdir) {
		    break;
		}
		$dir = trim($data);
		$d = "$this->pear_docdir/$this->destdir/$dir";
		if (is_file($d)) {
		    print "Error: wanted to create the directory $d\n";
		    print "       but it was already a file.\n";
		    break;
		}
		if (is_dir($d)) {
		    break;
		}
		if (!mkdir($d, 0755)) {
		    print "Error: could not mkdir $d\n";
		    break;
		}
		if ($this->debug) print "[debug] created directory $d\n";
		break;
	    case "DocFile":
		if (!$this->pear_docdir) {
		    break;
		}
		$file = trim($data);
		$d = "$this->pear_docdir/$this->destdir";
		if (!copy("$this->pkgdir/$file", "$d/$file")) {
		    print "Error: failed to copy $this->pkgdir/$file to $d\n";
		    break;
		}
		if ($this->debug) {
		    print "[debug] installed $d/$file\n";
		}
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
	include($file);
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

class PEAR_Installer_Error extends PEAR_Error {
    // {{{ constructor

    function PEAR_Installer_Error($msg) {
	$this->PEAR_Error($msg, 0, PEAR_ERROR_DIE);
    }

    // }}}
}

?>
