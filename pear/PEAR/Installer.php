<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
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
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR/Common.php";

/**
 * Administration class used to install PEAR packages and maintain the
 * installed package database.
 *
 * TODO:
 *  - move the package db handler methods to its own class.
 *  - install also the package.xml?
 *  - remove unneeded code
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR_Installer extends PEAR_Common
{
    // {{{ properties

    /** name of the package directory, for example Foo-1.0 */
    var $pkgdir;

    /** directory where PHP code files go */
    var $phpdir;

    /** directory where PHP extension files go */
    var $extdir;

    /** directory where documentation goes */
    var $docdir;

    /** directory where system state information goes */
    var $statedir;

    /** directory where the package wants to put files, relative
     *  to one of the three previous dirs
     */
    var $destdir = '';

    /** debug level (integer) */
    var $debug = 1;

    /** list of installed packages */
    var $pkglist = array();

    /** temporary directory */
    var $tmpdir;

    /** file pointer for package list file if open */
    var $pkglist_fp;

    // }}}

    // {{{ constructor

    function PEAR_Installer($phpdir = PEAR_INSTALL_DIR,
                            $extdir = PEAR_EXTENSION_DIR,
                            $docdir = '')
    {
        $this->PEAR();
        $this->phpdir = $phpdir;
        $this->extdir = $extdir;
        $this->docdir = $docdir;
        $this->statedir = "/var/lib/php"; // XXX FIXME Windows
    }

    // }}}
    // {{{ destructor

    function _PEAR_Installer()
    {
        chdir($this->pwd);
        if ($this->tmpdir && is_dir($this->tmpdir)) {
            system("rm -rf $this->tmpdir"); // XXX FIXME Windows
        }
        if (isset($this->pkglist_fp) && is_resource($this->pkglist_fp)) {
            flock($this->pkglist_fp, LOCK_UN);
            fclose($this->pkglist_fp);
        }
        $this->tmpdir = null;
        $this->pkglist_fp = null;
        $this->_PEAR_Common();
    }

    // }}}

    // {{{ lockPackageList()

    function lockPackageList()
    {
        $fp = $this->pkglist_fp;
        if (!is_resource($fp)) {
            $this->pkglist_fp = $fp = fopen($this->pkglist_file, "r");
        }
        return flock($fp, LOCK_EX);
    }

    // }}}
    // {{{ unlockPackageList()

    function unlockPackageList()
    {
        $fp = $this->pkglist_fp;
        if (!is_resource($fp)) {
            $this->pkglist_fp = $fp = fopen($this->pkglist_file, "r");
            $doclose = true;
        }
        $ret = flock($fp, LOCK_EX);
        if ($doclose) {
            fclose($fp);
        }
        return $ret;
    }

    // }}}
    // {{{ loadPackageList()

    function loadPackageList($file)
    {
        $this->pkglist_file = $file;
        $this->pkglist = array();
        if (!file_exists($file)) {
            if (!@touch($file)) {
                    return $this->raiseError("touch($file): $php_errormsg");
            }
        }
        $fp = $this->pkglist_fp = @fopen($file, "r");
        if (!is_resource($fp)) {
                return $this->raiseError("fopen($file): $php_errormsg");
        }
        $this->lockPackageList();
        $versionline = trim(fgets($fp, 2048));
        if ($versionline == ";1") {
            while ($line = fgets($fp, 2048)) {
                list($name, $version, $file) = explode(";", trim($line));
                $this->pkglist[$name]["version"] = $version;
                $this->pkglist[$name]["files"][] = $file;
            }
        }
        $this->unlockPackageList();
    }

    // }}}
    // {{{ savePackageList()

    function savePackageList()
    {
        $fp = $this->pkglist_fp;
        $wfp = @fopen($this->pkglist_file, "w");
        if (!is_resource($wfp)) {
            return $this->raiseError("could not write $this->pkglist_file");
        }
        if (is_resource($fp)) {
            fclose($fp);
        }
        $this->pkglist_fp = $fp = $wfp;
        fwrite($fp, ";1\n");
        foreach ($this->pkglist as $name => $entry) {
            $ver = $entry["version"];
            foreach ($entry["files"] as $file) {
                fwrite($fp, "$name;$ver;$file\n");
            }
        }
        fclose($fp);
        $this->pkglist_fp = $fp = null;
    }

    // }}}
    // {{{ updatePackageListFrom()

    function updatePackageListFrom($file)
    {
        /*
    $new = $this->classesDeclaredBy($file);
    reset($new);
    while (list($i, $name) = each($new)) {
        $this->pkglist['class'][$name] = $file;
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

    function install($pkgfile)
    {
        // XXX FIXME Add here code to manage packages database
        //$this->loadPackageList("$this->statedir/packages.lst");
        $this->pwd = getcwd();
        $need_download = false;
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            $need_download = true;
        } elseif (!@is_file($pkgfile)) {
            return $this->raiseError("$pkgfile: no such file");
        }
        // Download package -----------------------------------------------
        if ($need_download) {
            $file = basename($pkgfile);
            // XXX FIXME use ??? on Windows, use $TMPDIR on unix (use tmpnames?)
            $downloaddir = '/tmp/pearinstall';
            if (!$this->mkDirHier($downloaddir)) {
                return $this->raiseError("Failed to create tmp dir: $downloaddir");
            }
            $downloadfile = $downloaddir.DIRECTORY_SEPARATOR.$file;
            $this->log(1, "downloading $pkgfile ...");
            if (!$fp = @fopen($pkgfile, 'r')) {
                return $this->raiseError("$pkgfile: failed to download ($php_errormsg)");
            }
            if (!$wp = @fopen($downloadfile, 'w')) {
                return $this->raiseError("$downloadfile: write failed ($php_errormsg)");
            }
            $this->addTempFile($downloadfile);
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
            $this->log(1, '...done: ' . number_format($bytes, 0, '', ',') . ' bytes');
        }

        // Decompress pack in tmp dir -------------------------------------

        // To allow relative package file calls
        if (!chdir(dirname($pkgfile))) {
            return $this->raiseError('Unable to chdir to pakage directory');
        }
        $pkgfile = getcwd() . DIRECTORY_SEPARATOR . basename($pkgfile);

        // XXX FIXME Windows
        $this->tmpdir = tempnam('/tmp', 'pear');
        unlink($this->tmpdir);
        if (!mkdir($this->tmpdir, 0755)) {
            return $this->raiseError("Unable to create temporary directory $this->tmpdir.");
        } else {
            $this->log(2, '+ tmp dir created at ' . $this->tmpdir);
        }
        $this->addTempFile($this->tmpdir);
        if (!chdir($this->tmpdir)) {
            return $this->raiseError("Unable to chdir to $this->tmpdir.");
        }
        // XXX FIXME Windows
        $fp = popen("gzip -dc $pkgfile | tar -xvf -", 'r');
        $this->log(2, "+ launched command: gzip -dc $pkgfile | tar -xvf -");
        if (!is_resource($fp)) {
            return $this->raiseError("Unable to examine $pkgfile (gzip or tar failed)");
        }
        while ($line = fgets($fp, 4096)) {
            $line = rtrim($line);
            if (preg_match('!^[^/]+/package.xml$!', $line)) {
                if (isset($descfile)) {
                    return $this->raiseError("Invalid package: multiple package.xml files at depth one!");
                }
                $descfile = $line;
            }
        }
        pclose($fp);
        if (!isset($descfile) || !is_file($descfile)) {
            return $this->raiseError("No package.xml file after extracting the archive.");
        }

        // Parse xml file -----------------------------------------------
        $pkginfo = $this->infoFromDescriptionFile($descfile);
        if (PEAR::isError($pkginfo)) {
            return $pkginfo;
        }

        // Copy files to dest dir ---------------------------------------
        if (!is_dir($this->phpdir)) {
            return $this->raiseError("No script destination directory found\n",
                                     null, PEAR_ERROR_DIE);
        }
        $tmp_path = dirname($descfile);
        foreach ($pkginfo['filelist'] as $fname => $atts) {
            $dest_dir = $this->phpdir . DIRECTORY_SEPARATOR . dirname($fname);
            $fname = $tmp_path . DIRECTORY_SEPARATOR . $fname;
            $this->_copyFile($fname, $dest_dir, $atts);
        }
        return true;
    }

    function _copyFile($file, $dest_dir, $atts)
    {
        $type = strtolower($atts["Role"]);
        switch ($type) {
            case "test":
                // don't install test files for now
                $this->log(2, "+ Test file $file won't be installed yet");
                return true;
                break;
            case 'doc':
            case 'php':
            default:
                $dest_file = $dest_dir . DIRECTORY_SEPARATOR . basename($file);
                break;
        }
        if (!@is_dir($dest_dir)) {
            if (!$this->mkDirHier($dest_dir)) {
                $this->log(0, "failed to mkdir $dest_dir");
                return false;
            }
            $this->log(2, "+ created dir $dest_dir");
        }
        if (!@copy($file, $dest_file)) {
            $this->log(0, "failed to copy $file to $dest_file");
            return false;
        }
        $this->log(2, "+ copy $file to $dest_file");
        // FIXME Update Package database here
        //$this->updatePackageListFrom("$d/$file");
        $this->log(1, "installed file $dest_file");
        return true;
    }

    /* REMOVEME: This is the old code
    function _install($pkgfile)
    {
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            $need_download = true;
        } elseif (!file_exists($pkgfile)) {
            return $this->raiseError("$pkgfile: no such file");
        }

        if ($need_download) {
            $file = basename($pkgfile);
            // XXX FIXME use ??? on Windows, use $TMPDIR on unix
            $downloaddir = "/tmp/pearinstall";
            $this->mkDirHier($downloaddir);
            $downloadfile = $downloaddir.DIRECTORY_SEPARATOR.$file;
            $this->log(1, "downloading $pkgfile...");
            $fp = @fopen($pkgfile, "r");
            if (!$fp) {
                return $this->raiseError("$pkgfile: failed to download ($php_errormsg)");
            }
            $wp = @fopen($downloadfile, "w");
            if (!$wp) {
                return $this->raiseError("$downloadfile: write failed ($php_errormsg)");
            }
            $this->addTempFile($downloadfile);
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
        }
        // XXX FIXME depends on external gzip+tar
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

        // XXX FIXME Windows
        $this->tmpdir = tempnam("/tmp", "pear");
        unlink($this->tmpdir);
        if (!mkdir($this->tmpdir, 0755)) {
            return $this->raiseError("Unable to create temporary directory $this->tmpdir.");
        }
        $this->addTempFile($this->tmpdir);
        $pwd = getcwd();

        // XXX FIXME Windows should check for drive
        if (substr($pkgfile, 0, 1) == DIRECTORY_SEPARATOR) {
            $pkgfilepath = $pkgfile;
        } else {
            $pkgfilepath = $pwd.DIRECTORY_SEPARATOR.$pkgfile;
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
        $this->destdir = '';

        $data = fread($fp, filesize($descfile));
        if (!xml_parse($xp, $data, 1)) {
            $msg = sprintf("XML error: %s at line %d",
                           xml_error_string(xml_get_error_code($xp)),
                           xml_get_current_line_number($xp));
            xml_parser_free($xp);
            return $this->raiseError($msg);
        }

        xml_parser_free($xp);
        return true;
    }

    // }}}
    // {{{ startHandler()

    function startHandler($xp, $name, $attribs)
    {
        array_push($this->element_stack, $name);
        $this->current_element = $name;
        $this->current_attributes = $attribs;
        switch ($name) {
            case "Package":
                //if (strtolower($attribs["Type"]) != "binary") {
                //    return $this->raiseError("Invalid package: only binary packages supported yet.");
                //}
                //$this->pkginfo['pkgtype'] = strtolower($attribs["Type"]);
        break;
        }
    }

    // }}}
    // {{{ endHandler()

    function endHandler($xp, $name)
    {
        array_pop($this->element_stack);
        $this->current_element = $this->element_stack[sizeof($this->element_stack)-1];
    }

    // }}}
    // {{{ charHandler()

    function charHandler($xp, $data)
    {
        $next = $this->element_stack[sizeof($this->element_stack)-1];
        switch ($this->current_element) {
            case "Name":
                if ($next == "Package") {
                    $this->pkginfo["name"] = $data;
                }
                break;
            case "Version":
                $this->pkginfo["version"] = $data;
                break;
            case "Dir":
                if (!$this->phpdir) {
                    return $this->raiseError("No script destination directory found\n",
                                             null, PEAR_ERROR_DIE);
                    break;
                }
                $type = $this->current_attributes["Type"];
                $dir = trim($data);
                $d = "$this->phpdir/$this->destdir/$dir";
                if (substr($dir, 0, 1) == "/") {
                    $this->destdir = substr($dir, 1);
                } else {
                    $this->destdir = $dir;
                }
                break;

            case "File":
                if (!$this->phpdir) {
                    return $this->raiseError("No script destination directory found\n",
                                             null, PEAR_ERROR_DIE);
                    break;
                }
                $type = strtolower($this->current_attributes["Role"]);
                $file = trim($data);
                $updatepkglist = false;
                switch ($type) {
                    case "test":
                        $d = ""; // don't install test files for now
                        break;
                    default:
                        if ($this->destdir) {
                            $d = "$this->phpdir/$this->destdir";
                        } else {
                            $d = $this->phpdir;
                        }
                        $updatepkglist = true;
                        break;
                }
                if (!$d) {
                    break;
                }
                if (!@is_dir($d)) {
                    $this->mkDirHier($d);
                }
                $bfile = basename($file);
                if (!copy("$this->pkgdir/$file", "$d/$bfile")) {
                    $this->log(0, "failed to copy $this->pkgdir/$file to $d");
                    break;
                }
                if ($updatepkglist) {
                    $this->updatePackageListFrom("$d/$file");
                }
                $this->log(1, "installed $d/$bfile");
                break;
        }
    }
    */
    // }}}

    // {{{ classesDeclaredBy()

    /**
     * Find out which new classes are defined by a file.
     *
     * @param $file file name passed to "include"
     *
     * @return array classes that were defined
     */
    function classesDeclaredBy($file)
    {
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

    // {{{ declaredWhenIncluding()

    /**
     * Find out which new classes are defined by a file.
     *
     * @param $file file name passed to "include"
     *
     * @return array classes that were defined
     */
    function &declaredWhenIncluding($file)
    {
        $classes_before = get_declared_classes();
        $funcs_before = get_defined_functions();
//      $vars_before = $GLOBALS;
        ob_start();
        include($file);
        ob_end_clean();
        $classes_after = get_declared_classes();
        $funcs_after = get_defined_functions();
//      $vars_after = $GLOBALS;
        //using array_slice to renumber array
        return array(
            "classes" => array_slice(array_diff($classes_after, $classes_before), 0),
            "functions" => array_slice(array_diff($funcs_after, $funcs_before), 0),
//          "globals" => array_slice(array_diff($vars_after, $vars_before), 0)
        );
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
    function lockDir($dir)
    {
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

?>
