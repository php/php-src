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
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR/Common.php";

/**
 * Administration class used to install PEAR packages and maintain the
 * installed package database.
 *
 * TODO:
 *  - maintain file perms (look at umask or fileperms+chmod), ideas are welcome
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

    /** directory where the package wants to put files, relative
     *  to one of the three previous dirs
     */
    var $destdir = '';

    /** debug level (integer) */
    var $debug = 1;

    /** temporary directory */
    var $tmpdir;

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
        $this->tmpdir = null;
        $this->_PEAR_Common();
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
            return $this->raiseError("Could not open the package file: $pkgfile");
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
            $dest_dir = $this->phpdir . DIRECTORY_SEPARATOR;
            if (isset($atts['BaseInstallDir'])) {
                $dest_dir .= $atts['BaseInstallDir'] . DIRECTORY_SEPARATOR;
            }
            if (dirname($fname) != '.') {
                $dest_dir .= dirname($fname) . DIRECTORY_SEPARATOR;
            }
            $fname = $tmp_path . DIRECTORY_SEPARATOR . $fname;
            $this->_installFile($fname, $dest_dir, $atts);
        }
        return true;
    }

    function _installFile($file, $dest_dir, $atts)
    {
        $type = strtolower($atts['Role']);
        switch ($type) {
            case "test":
                // don't install test files for now
                $this->log(2, "+ Test file $file won't be installed yet");
                return true;
                break;
            case 'doc':
            case 'php':
            default:
                $dest_file = $dest_dir . basename($file);
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
    // }}}
}
?>