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
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Common.php';
require_once 'PEAR/Registry.php';
require_once 'PEAR/Dependency.php';

// TODO: remove empty dirs after uninstall

/**
 * Administration class used to install PEAR packages and maintain the
 * installed package database.
 *
 * TODO:
 *  - Install the "role=doc" files in a central pear doc dir
 *  - kill FIXME's
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR_Installer extends PEAR_Common
{
    // {{{ properties

    /** name of the package directory, for example Foo-1.0
     * @var string
     */
    var $pkgdir;

    /** directory where PHP code files go
     * @var string
     */
    var $phpdir;

    /** directory where PHP extension files go
     * @var string
     */
    var $extdir;

    /** directory where documentation goes
     * @var string
     */
    var $docdir;

    /** directory where the package wants to put files, relative
     *  to one of the previous dirs
     * @var string
     */
    var $destdir = '';

    /** debug level
     * @var int
     */
    var $debug = 1;

    /** temporary directory
     * @var string
     */
    var $tmpdir;

    /** PEAR_Registry object used by the installer
     * @var object
     */
    var $registry;

    /** PEAR_Config object used by the installer
     * @var object
     */
    var $config;

    // }}}

    // {{{ constructor

    function PEAR_Installer(&$config)
    {
        $this->PEAR();
        $this->config = &$config;
    }

    // }}}

    // {{{ _deletePackageFiles()

    function _deletePackageFiles($package)
    {
        if (!strlen($package)) {
            return $this->raiseError("No package to uninstall given");
        }
        $filelist = $this->registry->packageInfo($package, 'filelist');
        if ($filelist == null) {
            return $this->raiseError("$package not installed");
        }
        foreach ($filelist as $file => $props) {
            $path = $props['installed_as'];
            if (!@unlink($path)) {
                $this->log(2, "unable to delete: $path");
            } else {
                @rmdir(dirname($path)); // Delete package directory if it's empty
                $this->log(2, "+ deleted file: $path");
            }
        }
    }

    // }}}
    // {{{ _installFile()

    function _installFile($file, $atts, $tmp_path)
    {
        switch ($atts['role']) {
            case 'test': case 'data': case 'ext':
                // don't install test files for now
                $this->log(2, "+ $file: $atts[role] file not installed yet");
                return true;
            case 'doc':
                $dest_dir = $this->config->get('doc_dir') .
                     DIRECTORY_SEPARATOR . $this->pkginfo['package'];
                break;
            case 'extsrc':
                // don't install test files for now
                $this->log(2, "+ $file: no support for building extensions yet");
                return true;
            case 'php':
                $dest_dir = $this->config->get('php_dir');
                break;
            case 'script': {
                $dest_dir = $this->config->get('bin_dir');
                break;
            }
            default:
                break;
        }
        if (isset($atts['baseinstalldir']) && $atts['role'] != 'doc') {
            $dest_dir .= DIRECTORY_SEPARATOR . $atts['baseinstalldir'];
        }
        if (dirname($file) != '.') {
            $dest_dir .= DIRECTORY_SEPARATOR . dirname($file);
        }
        if (empty($atts['install-as'])) {
            $dest_file = $dest_dir . DIRECTORY_SEPARATOR . basename($file);
        } else {
            $dest_file = $dest_dir . DIRECTORY_SEPARATOR . $atts['install-as'];
        }
        $dest_file = preg_replace('!//+!', '/', $dest_file);
        if (!@is_dir($dest_dir)) {
            if (!$this->mkDirHier($dest_dir)) {
                $this->log(0, "failed to mkdir $dest_dir");
                return false;
            }
            $this->log(2, "+ created dir $dest_dir");
        }
        $orig_file = $tmp_path . DIRECTORY_SEPARATOR . $file;
        if (empty($atts['replacements'])) {
            if (!@copy($orig_file, $dest_file)) {
                $this->log(0, "failed to copy $orig_file to $dest_file");
                return false;
            }
            $this->log(2, "+ copy $orig_file to $dest_file");
        } else {
            $fp = fopen($orig_file, "r");
            $contents = fread($fp, filesize($orig_file));
            fclose($fp);
            $subst_from = $subst_to = array();
            foreach ($atts['replacements'] as $a) {
                $to = '';
                if ($a['type'] == 'php-const') {
                    if (preg_match('/^[a-z0-9_]+$/i', $a['to'])) {
                        eval("\$to = $a[to];");
                    } else {
                        $this->log(0, "invalid php-const replacement: $a[to]");
                        continue;
                    }
                } elseif ($a['type'] == 'pear-config') {
                    $to = $this->config->get($a['to']);
                }
                if ($to) {
                    $subst_from = $a['from'];
                    $subst_to = $to;
                }
            }
            if (sizeof($subst_from)) {
                $contents = str_replace($subst_from, $subst_to, $contents);
            }
            $wp = @fopen($dest_file, "w");
            if (!is_resource($wp)) {
                $this->log(0, "failed to create $dest_file");
                return false;
            }
            fwrite($wp, $contents);
            fclose($wp);
        }
        if (!OS_WINDOWS) {
            if ($atts['role'] == 'script') {
                $mode = 0755;
            } else {
                $mode = 0644;
            }
            if (!@chmod($dest_file, $mode)) {
                $this->log(0, "failed to change mode of $dest_file");
            }
        }

        // Store the full path where the file was installed for easy unistall
        $this->pkginfo['filelist'][$file]['installed_as'] = $dest_file;

        $this->log(1, "installed file $dest_file");
        return true;
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

    function install($pkgfile, $options = array(), $config = null)
    {
        // recognized options:
        // - register_only : update registry but don't install files
        // - upgrade       : upgrade existing install
        // - soft          : fail silently
        //
        if (empty($this->registry)) {
            $this->registry = &new PEAR_Registry($this->config->get('php_dir'));
        }
        $oldcwd = getcwd();
        $need_download = false;
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            $need_download = true;
        } elseif (!@is_file($pkgfile)) {
            if (preg_match('/^[A-Z][A-Za-z0-9_]+$/', $pkgfile)) {
                // valid package name
                if ($this->registry->packageExists($pkgfile)) {
                    return $this->raiseError("$pkgfile already installed");
                }
                if ($config === null) {
                    $pkgfile = "http://pear.php.net/get/$pkgfile";
                } else {
                    $pkgfile = "http://" . $config->get('master_server') .
                         "/get/$pkgfile";
                }
                if (!extension_loaded("zlib")) {
                    $pkgfile .= '?uncompress=yes';
                }
                $need_download = true;
            } else {
                if (strlen($pkgfile)) {
                    return $this->raiseError("Could not open the package file: $pkgfile");
                } else {
                    return $this->raiseError("No package file given");
                }
            }
        }

        // Download package -----------------------------------------------
        if ($need_download) {
            $file = basename($pkgfile);
            if (PEAR::isError($downloaddir = $this->mkTempDir())) {
                return $downloaddir;
            }
            $this->log(2, '+ tmp dir created at ' . $downloaddir);
            $downloadfile = $downloaddir . DIRECTORY_SEPARATOR . $file;
            $this->log(1, "downloading $pkgfile ...");
            if (!$fp = @fopen($pkgfile, 'r')) {
                return $this->raiseError("$pkgfile: failed to download ($php_errormsg)");
            }
            // XXX FIXME should check content-disposition header
            // for now we set the "force" option to avoid an error
            // because of a temp. package file called "Package"
            if (!$wp = @fopen($downloadfile, 'w')) {
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
            $this->log(1, '...done: ' . number_format($bytes, 0, '', ',') . ' bytes');
        }

        if (substr($pkgfile, -4) == '.xml') {
            $descfile = $pkgfile;
        } else {
            // Decompress pack in tmp dir -------------------------------------

            // To allow relative package file calls
            if (!chdir(dirname($pkgfile))) {
                return $this->raiseError('unable to chdir to package directory');
            }
            $pkgfile = getcwd() . DIRECTORY_SEPARATOR . basename($pkgfile);

            if (PEAR::isError($tmpdir = $this->mkTempDir())) {
                chdir($oldcwd);
                return $tmpdir;
            }
            $this->log(2, '+ tmp dir created at ' . $tmpdir);

            $tar = new Archive_Tar($pkgfile, true);
            if (!@$tar->extract($tmpdir)) {
                chdir($oldcwd);
                return $this->raiseError("unable to unpack $pkgfile");
            }

            // ----- Look for existing package file
            $descfile = $tmpdir . DIRECTORY_SEPARATOR . 'package.xml';

            //  ==> XXX This part should be removed later on
            $flag_old_format = false;
            if (!is_file($descfile)) {
                // ----- Look for old package archive format
                // In this format the package.xml file was inside the
                // Package-n.n directory
                $dp = opendir($tmpdir);
                do {
                    $pkgdir = readdir($dp);
                } while ($pkgdir{0} == '.');

                $descfile = $tmpdir . DIRECTORY_SEPARATOR . $pkgdir . DIRECTORY_SEPARATOR . 'package.xml';
                $flag_old_format = true;
                $this->log(0, "warning : you are using an archive with an old format");
            }
            // <== XXX This part should be removed later on
        }

        if (!is_file($descfile)) {
            chdir($oldcwd);
            return $this->raiseError("no package.xml file after extracting the archive");
        }

        // Parse xml file -----------------------------------------------
        $pkginfo = $this->infoFromDescriptionFile($descfile);
        if (PEAR::isError($pkginfo)) {
            chdir($oldcwd);
            return $pkginfo;
        }

        $pkgname = $pkginfo['package'];

        // Check dependencies -------------------------------------------
        if (isset($pkginfo['release_deps']) && !isset($options['nodeps'])) {
            $error = $this->checkDeps($pkginfo);
            if ($error) {
                if (empty($options['soft'])) {
                    $this->log(0, $error);
                }
                return $this->raiseError('Dependencies failed');
            }
        }

        if (empty($options['upgrade'])) {
            // checks to do only when installing new packages
            if (empty($options['force']) && $this->registry->packageExists($pkgname)) {
                return $this->raiseError("$pkgname already installed");
            }
        } else {
            // check to do only when upgrading packages
            if (!$this->registry->packageExists($pkgname)) {
                return $this->raiseError("$pkgname not installed");
            }
            $v1 = $this->registry->packageInfo($pkgname, 'version');
            $v2 = $pkginfo['version'];
            $cmp = version_compare($v1, $v2, 'gt');
            if (empty($options['force']) && !version_compare($v2, $v1, 'gt')) {
                return $this->raiseError("upgrade to a newer version ($v2 is not newer than $v1)");
            }
            if (empty($options['register_only'])) {
                // when upgrading, remove old release's files first:
                $this->_deletePackageFiles($pkgname);
            }
        }

        // Copy files to dest dir ---------------------------------------

        // info from the package it self we want to access from _installFile
        $this->pkginfo = $pkginfo;
        if (empty($options['register_only'])) {
            if (!is_dir($this->config->get('php_dir'))) {
                chdir($oldcwd);
                return $this->raiseError("no script destination directory\n",
                                         null, PEAR_ERROR_DIE);
            }

            // don't want strange characters
            $pkgname    = ereg_replace ('[^a-zA-Z0-9._]', '_', $pkginfo['package']);
            $pkgversion = ereg_replace ('[^a-zA-Z0-9._\-]', '_', $pkginfo['version']);
            $tmp_path = dirname($descfile);
            if (substr($pkgfile, -4) != '.xml') {
                $tmp_path .= DIRECTORY_SEPARATOR . $pkgname . '-' . $pkgversion;
            }

            //  ==> XXX This part should be removed later on
            if ($flag_old_format) {
                $tmp_path = dirname($descfile);
            }
            // <== XXX This part should be removed later on

            foreach ($pkginfo['filelist'] as $file => $atts) {
                $this->_installFile($file, $atts, $tmp_path);
            }
        }

        // Register that the package is installed -----------------------
        if (empty($options['upgrade'])) {
            // if 'force' is used, replace the info in registry
            if (!empty($options['force']) && $this->registry->packageExists($pkgname)) {
                $this->registry->deletePackage($pkgname);
            }
            $ret = $this->registry->addPackage($pkgname, $this->pkginfo);
        } else {
            $ret = $this->registry->updatePackage($pkgname, $this->pkginfo, false);
        }
        chdir($oldcwd);
        return $ret;
    }

    // }}}
    // {{{ uninstall()

    function uninstall($package)
    {
        if (empty($this->registry)) {
            $this->registry = new PEAR_Registry($this->config->get('php_dir'));
        }

        // Delete the files
        $this->_deletePackageFiles($package);

        // Register that the package is no longer installed
        return $this->registry->deletePackage($package);
    }

    // }}}
    // {{{ checkDeps()

    function checkDeps(&$pkginfo)
    {
        $deps = &new PEAR_Dependency($this->registry);
        $errors = null;
        if (is_array($pkginfo['release_deps'])) {
            foreach($pkginfo['release_deps'] as $dep) {
                if ($error = $deps->callCheckMethod($dep)) {
                    $errors .= "\n$error";
                }
            }
            if ($errors) {
                return $errors;
            }
        }
        return false;
    }

    // }}}
}

?>
