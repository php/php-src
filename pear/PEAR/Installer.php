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
require_once 'System.php';

define('PEAR_INSTALLER_OK',       1);
define('PEAR_INSTALLER_FAILED',   0);
define('PEAR_INSTALLER_SKIPPED', -1);

/**
 * Administration class used to install PEAR packages and maintain the
 * installed package database.
 *
 * TODO:
 *   - Check dependencies break on package uninstall (when no force given)
 *   - add a guessInstallDest() method with the code from _installFile() and
 *     use that method in Registry::_rebuildFileMap() & Command_Registry::doList(),
 *     others..
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

    // }}}

    // {{{ constructor

    /**
     * PEAR_Installer constructor.
     *
     * @param object $ui user interface object (instance of PEAR_Frontend_*)
     *
     * @access public
     */
    function PEAR_Installer(&$ui)
    {
        $this->PEAR_Common();
        $this->setFrontendObject($ui);
        $this->debug = $this->config->get('verbose');
        $this->registry = &new PEAR_Registry($this->config->get('php_dir'));
    }

    // }}}

    // {{{ _deletePackageFiles()

    /**
     * Delete a package's installed files, remove empty directories.
     *
     * @param string $package package name
     *
     * @return bool TRUE on success, or a PEAR error on failure
     *
     * @access private
     */
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
            if (empty($props['installed_as'])) {
                continue;
            }
            $path = $props['installed_as'];
            if (!@unlink($path)) {
                $this->log(2, "unable to delete: $path");
            } else {
                $this->log(2, "deleted file $path");
                // Delete package directory if it's empty
                if (@rmdir(dirname($path))) {
                    $this->log(3, "+ rmdir $path");
                }
            }
        }
        return true;
    }

    // }}}
    // {{{ _installFile()

    function _installFile($file, $atts, $tmp_path)
    {
        static $os;
        if (isset($atts['platform'])) {
            if (empty($os)) {
                include_once "OS/Guess.php";
                $os = new OS_Guess();
            }
            // return if this file is meant for another platform
            if (!$os->matchSignature($atts['platform'])) {
                $this->log(2, "skipped $file (meant for $atts[platform], we are ".$os->getSignature().")");
                return PEAR_INSTALLER_SKIPPED;
            }
        }

        switch ($atts['role']) {
            case 'doc':
            case 'data':
            case 'test':
                $dest_dir = $this->config->get($atts['role'] . '_dir') .
                            DIRECTORY_SEPARATOR . $this->pkginfo['package'];
                unset($atts['baseinstalldir']);
                break;
            case 'ext':
            case 'php':
                $dest_dir = $this->config->get($atts['role'] . '_dir');
                break;
            case 'script':
                $dest_dir = $this->config->get('bin_dir');
                break;
            case 'src':
            case 'extsrc':
                $this->source_files++;
                return;
            default:
                // Files with no role will end in "/"
                return $this->raiseError("Invalid role `$atts[role]' for file $file");
        }
        if (!empty($atts['baseinstalldir'])) {
            $dest_dir .= DIRECTORY_SEPARATOR . $atts['baseinstalldir'];
        }
        if (dirname($file) != '.' && empty($atts['install-as'])) {
            $dest_dir .= DIRECTORY_SEPARATOR . dirname($file);
        }
        if (empty($atts['install-as'])) {
            $dest_file = $dest_dir . DIRECTORY_SEPARATOR . basename($file);
        } else {
            $dest_file = $dest_dir . DIRECTORY_SEPARATOR . $atts['install-as'];
        }
        $orig_file = $tmp_path . DIRECTORY_SEPARATOR . $file;

        // Clean up the DIRECTORY_SEPARATOR mess
        $ds2 = str_repeat(DIRECTORY_SEPARATOR, 2);
        list($dest_file, $orig_file) = preg_replace(array('!\\\\!', '!/!', "!$ds2+!"),
                                                    DIRECTORY_SEPARATOR,
                                                    array($dest_file, $orig_file));
        $dest_dir = dirname($dest_file);
        if (!@is_dir($dest_dir)) {
            if (!$this->mkDirHier($dest_dir)) {
                return $this->raiseError("failed to mkdir $dest_dir",
                                         PEAR_INSTALLER_FAILED);
            }
            $this->log(3, "+ mkdir $dest_dir");
        }
        if (empty($atts['replacements'])) {
            if (!@copy($orig_file, $dest_file)) {
                return $this->raiseError("failed to copy $orig_file to $dest_file",
                                         PEAR_INSTALLER_FAILED);
            }
            $this->log(3, "+ cp $orig_file $dest_file");
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
                } elseif ($a['type'] == 'package-info') {
                    $to = $this->pkginfo[$a['to']];
                }
                if ($to) {
                    $subst_from[] = $a['from'];
                    $subst_to[] = $to;
                }
            }
            $this->log(2, "doing ".sizeof($subst_from)." substitution(s) for $dest_file");
            if (sizeof($subst_from)) {
                $contents = str_replace($subst_from, $subst_to, $contents);
            }
            $wp = @fopen($dest_file, "w");
            if (!is_resource($wp)) {
                return $this->raiseError("failed to create $dest_file",
                                         PEAR_INSTALLER_FAILED);
            }
            fwrite($wp, $contents);
            fclose($wp);
        }
        if (!OS_WINDOWS) {
            if ($atts['role'] == 'script') {
                $mode = 0777 & ~(int)octdec($this->config->get('umask'));
                $this->log(3, "+ chmod +x $dest_file");
            } else {
                $mode = 0666 & ~(int)octdec($this->config->get('umask'));
            }
            if (!@chmod($dest_file, $mode)) {
                $this->log(0, "failed to change mode of $dest_file");
            }
        }

        // Store the full path where the file was installed for easy unistall
        $this->pkginfo['filelist'][$file]['installed_as'] = $dest_file;

        $this->log(2, "installed file $dest_file");
        return PEAR_INSTALLER_OK;
    }

    // }}}
    // {{{ getPackageDownloadUrl()

    function getPackageDownloadUrl($package)
    {
        if ($this === null || $this->config === null) {
            $package = "http://pear.php.net/get/$package";
        } else {
            $package = "http://" . $this->config->get('master_server') .
                "/get/$package";
        }
        if (!extension_loaded("zlib")) {
            $package .= '?uncompress=yes';
        }
        return $package;
    }

    // }}}
    // {{{ install()

    /**
     * Installs the files within the package file specified.
     *
     * @param $pkgfile path to the package file
     *
     * @return array package info if successful, null if not
     */

    function install($pkgfile, $options = array())
    {
        // recognized options:
        // - force         : force installation
        // - register-only : update registry but don't install files
        // - upgrade       : upgrade existing install
        // - soft          : fail silently
        //
        $need_download = false;
        //  ==> XXX should be removed later on
        $flag_old_format = false;
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            $need_download = true;
        } elseif (!@is_file($pkgfile)) {
            if ($this->validPackageName($pkgfile)) {
                if ($this->registry->packageExists($pkgfile) &&
                    empty($options['upgrade']) && empty($options['force']))
                {
                    return $this->raiseError("$pkgfile already installed");
                }
                $pkgfile = $this->getPackageDownloadUrl($pkgfile);
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
            $downloaddir = $this->config->get('download_dir');
            if (empty($downloaddir)) {
                if (PEAR::isError($downloaddir = System::mktemp('-d'))) {
                    return $downloaddir;
                }
                $this->log(2, '+ tmp dir created at ' . $downloaddir);
            }
            $callback = $this->ui ? array(&$this, '_downloadCallback') : null;
            $file = $this->downloadHttp($pkgfile, $this->ui, $downloaddir, $callback);
            if (PEAR::isError($file)) {
                return $this->raiseError($file);
            }
            $pkgfile = $file;
        }

        if (substr($pkgfile, -4) == '.xml') {
            $descfile = $pkgfile;
        } else {
            // Decompress pack in tmp dir -------------------------------------

            // To allow relative package file names
            $oldcwd = getcwd();
            if (@chdir(dirname($pkgfile))) {
                $pkgfile = getcwd() . DIRECTORY_SEPARATOR . basename($pkgfile);
                chdir($oldcwd);
            }

            if (PEAR::isError($tmpdir = System::mktemp('-d'))) {
                return $tmpdir;
            }
            $this->log(2, '+ tmp dir created at ' . $tmpdir);

            $tar = new Archive_Tar($pkgfile);
            if (!@$tar->extract($tmpdir)) {
                return $this->raiseError("unable to unpack $pkgfile");
            }

            // ----- Look for existing package file
            $descfile = $tmpdir . DIRECTORY_SEPARATOR . 'package.xml';

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
            return $this->raiseError("no package.xml file after extracting the archive");
        }

        // Parse xml file -----------------------------------------------
        $pkginfo = $this->infoFromDescriptionFile($descfile);
        if (PEAR::isError($pkginfo)) {
            return $pkginfo;
        }
        $this->validatePackageInfo($pkginfo, $errors, $warnings);
        // XXX We allow warnings, do we have to do it?
        if (count($errors)) {
            if (empty($options['force'])) {
                return $this->raiseError("The following errors where found (use force option to install anyway):\n".
                                         implode("\n", $errors));
            } else {
                $this->log(0, "warning : the following errors were found:\n".
                           implode("\n", $errors));
            }
        }

        $pkgname = $pkginfo['package'];

        // Check dependencies -------------------------------------------
        if (isset($pkginfo['release_deps']) && empty($options['nodeps'])) {
            $error = $this->checkDeps($pkginfo);
            if ($error) {
                if (empty($options['soft'])) {
                    $this->log(0, $error);
                }
                return $this->raiseError("$pkgname: dependencies failed");
            }
        }

        if (empty($options['force'])) {
            // checks to do when not in "force" mode
            $test = $this->registry->checkFileMap($pkginfo);
            if (sizeof($test)) {
                $tmp = $test;
                foreach ($tmp as $file => $pkg) {
                    if ($pkg == $pkgname) {
                        unset($test[$file]);
                    }
                }
                if (sizeof($test)) {
                    $msg = "$pkgname: conflicting files found:\n";
                    $longest = max(array_map("strlen", array_keys($test)));
                    $fmt = "%${longest}s (%s)\n";
                    foreach ($test as $file => $pkg) {
                        $msg .= sprintf($fmt, $file, $pkg);
                    }
                    return $this->raiseError($msg);
                }
            }
        }

        if (empty($options['upgrade'])) {
            // checks to do only when installing new packages
            if (empty($options['force']) && $this->registry->packageExists($pkgname)) {
                return $this->raiseError("$pkgname already installed");
            }
        } else {
            // checks to do only when upgrading packages
            if (!$this->registry->packageExists($pkgname)) {
                return $this->raiseError("$pkgname not installed");
            }
            $v1 = $this->registry->packageInfo($pkgname, 'version');
            $v2 = $pkginfo['version'];
            $cmp = version_compare("$v1", "$v2", 'gt');
            if (empty($options['force']) && !version_compare("$v2", "$v1", 'gt')) {
                return $this->raiseError("upgrade to a newer version ($v2 is not newer than $v1)");
            }
            if (empty($options['register-only'])) {
                // when upgrading, remove old release's files first:
                if (PEAR::isError($err = $this->_deletePackageFiles($pkgname))) {
                    return $this->raiseError($err);
                }
            }
        }

        // Copy files to dest dir ---------------------------------------

        // info from the package it self we want to access from _installFile
        $this->pkginfo = &$pkginfo;
        // used to determine whether we should build any C code
        $this->source_files = 0;

        if (empty($options['register-only'])) {
            if (!is_dir($this->config->get('php_dir'))) {
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
                $this->expectError(PEAR_INSTALLER_FAILED);
                $res = $this->_installFile($file, $atts, $tmp_path);
                $this->popExpect();
                if (PEAR::isError($res)) {
                    if (empty($options['force'])) {
                        return $this->raiseError($res);
                    } else {
                        $this->log(0, "Warning: " . $res->getMessage());
                    }
                }
                if ($res != PEAR_INSTALLER_OK) {
                    // Do not register files that were not installed
                    unset($pkginfo['filelist'][$file]);
                }
            }

            if ($this->source_files > 0 && empty($options['nobuild'])) {
                $this->log(1, "$this->source_files source files, building");
                $bob = &new PEAR_Builder($this->ui);
                $bob->debug = $this->debug;
                $built = $bob->build($descfile, array(&$this, '_buildCallback'));
                if (PEAR::isError($built)) {
                    return $built;
                }
                foreach ($built as $ext) {
                    $bn = basename($ext['file']);
                    $this->log(2, "installing $bn");
                    $dest = $this->config->get('ext_dir') .
                        DIRECTORY_SEPARATOR . $bn;
                    $this->log(3, "+ cp $ext[file] ext_dir");
                    if (!@copy($ext['file'], $dest)) {
                        return $this->raiseError("failed to copy $bn to $dest");
                    }
                    $pkginfo['filelist'][$bn] = array(
                        'role' => 'ext',
                        'installed_as' => $dest,
                        'php_api' => $ext['php_api'],
                        'zend_mod_api' => $ext['zend_mod_api'],
                        'zend_ext_api' => $ext['zend_ext_api'],
                        );
                }
            }
        }

        // Register that the package is installed -----------------------
        if (empty($options['upgrade'])) {
            // if 'force' is used, replace the info in registry
            if (!empty($options['force']) && $this->registry->packageExists($pkgname)) {
                $this->registry->deletePackage($pkgname);
            }
            $ret = $this->registry->addPackage($pkgname, $pkginfo);
        } else {
            $ret = $this->registry->updatePackage($pkgname, $pkginfo, false);
        }
        if (!$ret) {
            return null;
        }
        return $pkginfo;
    }

    // }}}
    // {{{ uninstall()

    function uninstall($package)
    {
        if (empty($this->registry)) {
            $this->registry = new PEAR_Registry($this->config->get('php_dir'));
        }

        // Delete the files
        if (PEAR::isError($err = $this->_deletePackageFiles($package))) {
            return $this->raiseError($err);
        }

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
    // {{{ _downloadCallback()

    function _downloadCallback($msg, $params = null)
    {
        switch ($msg) {
            case 'saveas':
                $this->log(1, "downloading $params ...");
                break;
            case 'done':
                $this->log(1, '...done: ' . number_format($params, 0, '', ',') . ' bytes');
                break;
        }
        if (method_exists($this->ui, '_downloadCallback'))
            $this->ui->_downloadCallback($msg, $params);
    }

    // }}}
    // {{{ _buildCallback()

    function _buildCallback($what, $data)
    {
        if (($what == 'cmdoutput' && $this->debug > 1) ||
            ($what == 'output' && $this->debug > 0)) {
            $this->ui->outputData(rtrim($data), 'build');
        }
    }

    // }}}
}

?>
