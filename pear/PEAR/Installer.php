<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
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

    /** installation root directory (ala PHP's INSTALL_ROOT or
     * automake's DESTDIR
     * @var string
     */
    var $installroot = '';

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

    /** List of file transactions queued for an install/upgrade/uninstall.
     *
     *  Format:
     *    array(
     *      0 => array("rename => array("from-file", "to-file")),
     *      1 => array("delete" => array("file-to-delete")),
     *      ...
     *    )
     *
     * @var array
     */
    var $file_operations = array();

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
        parent::PEAR_Common();
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
            $path = $this->_prependPath($props['installed_as'], $this->installroot);
            $this->addFileOperation('delete', array($path));
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
                $this->log(3, "skipped $file (meant for $atts[platform], we are ".$os->getSignature().")");
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
        $ds2 = DIRECTORY_SEPARATOR . DIRECTORY_SEPARATOR;
        list($dest_file, $orig_file) = preg_replace(array('!\\\\+!', '!/!', "!$ds2+!"),
                                                    DIRECTORY_SEPARATOR,
                                                    array($dest_file, $orig_file));
        $installed_as = $dest_file;
        $final_dest_file = $this->_prependPath($dest_file, $this->installroot);
        $dest_dir = dirname($final_dest_file);
        $dest_file = $dest_dir . DIRECTORY_SEPARATOR . '.tmp' . basename($final_dest_file);
        if (!@is_dir($dest_dir)) {
            if (!$this->mkDirHier($dest_dir)) {
                return $this->raiseError("failed to mkdir $dest_dir",
                                         PEAR_INSTALLER_FAILED);
            }
            $this->log(3, "+ mkdir $dest_dir");
        }
        if (empty($atts['replacements'])) {
            if (!@copy($orig_file, $dest_file)) {
                return $this->raiseError("failed to write $dest_file",
                                         PEAR_INSTALLER_FAILED);
            }
            $this->log(3, "+ cp $orig_file $dest_file");
            if (isset($atts['md5sum'])) {
                $md5sum = md5_file($dest_file);
            }
        } else {
            $fp = fopen($orig_file, "r");
            $contents = fread($fp, filesize($orig_file));
            fclose($fp);
            if (isset($atts['md5sum'])) {
                $md5sum = md5($contents);
            }
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
            $this->log(3, "doing ".sizeof($subst_from)." substitution(s) for $final_dest_file");
            if (sizeof($subst_from)) {
                $contents = str_replace($subst_from, $subst_to, $contents);
            }
            $wp = @fopen($dest_file, "w");
            if (!is_resource($wp)) {
                return $this->raiseError("failed to create $dest_file: $php_errormsg",
                                         PEAR_INSTALLER_FAILED);
            }
            if (!fwrite($wp, $contents)) {
                return $this->raiseError("failed writing to $dest_file: $php_errormsg",
                                         PEAR_INSTALLER_FAILED);
            }
            fclose($wp);
        }
        if (isset($md5sum)) {
            if ($md5sum == $atts['md5sum']) {
                $this->log(3, "md5sum ok: $final_dest_file");
            } else {
                $this->log(0, "warning : bad md5sum for file $final_dest_file");
            }
        }
        if (!OS_WINDOWS) {
            if ($atts['role'] == 'script') {
                $mode = 0777 & ~(int)octdec($this->config->get('umask'));
                $this->log(3, "+ chmod +x $dest_file");
            } else {
                $mode = 0666 & ~(int)octdec($this->config->get('umask'));
            }
            $this->addFileOperation("chmod", array($mode, $dest_file));
            if (!@chmod($dest_file, $mode)) {
                $this->log(0, "failed to change mode of $dest_file");
            }
        }
        $this->addFileOperation("rename", array($dest_file, $final_dest_file));

        // XXX SHOULD BE DONE ONLY AFTER COMMIT
        // Store the full path where the file was installed for easy unistall
        $this->pkginfo['filelist'][$file]['installed_as'] = $installed_as;

        //$this->log(2, "installed: $dest_file");
        return PEAR_INSTALLER_OK;
    }

    // }}}
    // {{{ addFileOperation()

    function addFileOperation($type, $data)
    {
        if ($type == 'chmod') {
            $octmode = decoct($data[0]);
            $this->log(3, "adding to transaction: $type $octmode $data[1]");
        } else {
            $this->log(3, "adding to transaction: $type " . implode(" ", $data));
        }
        $this->file_operations[] = array($type, $data);
    }

    // }}}
    // {{{ startFileTransaction()

    function startFileTransaction($rollback_in_case = false)
    {
        if (count($this->file_operations) && $rollback_in_case) {
            $this->rollbackFileTransaction();
        }
        $this->file_operations = array();
    }

    // }}}
    // {{{ commitFileTransaction()

    function commitFileTransaction()
    {
        $n = count($this->file_operations);
        $this->log(2, "about to commit $n file operations");
        // first, check permissions and such manually
        $errors = array();
        foreach ($this->file_operations as $tr) {
            list($type, $data) = $tr;
            switch ($type) {
                case 'rename':
                    // check that dest dir. is writable
                    if (!is_writable(dirname($data[1]))) {
                        $errors[] = "permission denied ($type): $data[1]";
                    }
                    break;
                case 'chmod':
                    // check that file is writable
                    if (!is_writable($data[1])) {
                        $errors[] = "permission denied ($type): $data[1]";
                    }
                    break;
                case 'delete':
                    // check that directory is writable
                    if (file_exists($data[0]) && !is_writable(dirname($data[0]))) {
                        $errors[] = "permission denied ($type): $data[0]";
                    }
                    break;
            }

        }
        $m = sizeof($errors);
        if ($m > 0) {
            foreach ($errors as $error) {
                $this->log(1, $error);
            }
            return false;
        }
        // really commit the transaction
        foreach ($this->file_operations as $tr) {
            list($type, $data) = $tr;
            switch ($type) {
                case 'rename':
                    @rename($data[0], $data[1]);
                    $this->log(3, "+ mv $data[0] $data[1]");
                    break;
                case 'chmod':
                    @chmod($data[0], $data[1]);
                    $octmode = decoct($data[0]);
                    $this->log(3, "+ chmod $octmode $data[1]");
                    break;
                case 'delete':
                    @unlink($data[0]);
                    $this->log(3, "+ rm $data[0]");
                    break;
                case 'rmdir':
                    @rmdir($data[0]);
                    $this->log(3, "+ rmdir $data[0]");
                    break;
            }
        }
        $this->log(2, "successfully commited $n file operations");
        $this->file_operations = array();
        return true;
    }

    // }}}
    // {{{ rollbackFileTransaction()

    function rollbackFileTransaction()
    {
        $n = count($this->file_operations);
        $this->log(2, "rolling back $n file operations");
        foreach ($this->file_operations as $tr) {
            list($type, $data) = $tr;
            switch ($type) {
                case 'rename':
                    @unlink($data[0]);
                    $this->log(3, "+ rm $data[0]");
                    break;
                case 'mkdir':
                    @rmdir($data[0]);
                    $this->log(3, "+ rmdir $data[0]");
                    break;
                case 'chmod':
                    break;
                case 'delete':
                    break;
            }
        }
        $this->file_operations = array();
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
    // {{{ mkDirHier($dir)

    function mkDirHier($dir)
    {
        $this->addFileOperation('mkdir', array($dir));
        return parent::mkDirHier($dir);
    }

    // }}}

    // {{{ _prependPath($path, $prepend)

    function _prependPath($path, $prepend)
    {
        if (OS_WINDOWS && preg_match('/^[a-z]:/i', $path)) {
            $path = $prepend . substr($path, 2);
        } else {
            $path = $prepend . $path;
        }
        return $path;
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
        $php_dir = $this->config->get('php_dir');
        if (isset($options['installroot'])) {
            if (substr($options['installroot'], -1) == DIRECTORY_SEPARATOR) {
                $options['installroot'] = substr($options['installroot'], 0, -1);
            }
            $php_dir = $this->_prependPath($php_dir, $options['installroot']);
            $this->registry = &new PEAR_Registry($php_dir);
            $this->installroot = $options['installroot'];
        } else {
            $registry = &$this->registry;
            $this->installroot = '';
        }
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
            if (!is_dir($php_dir)) {
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
                    if (empty($options['ignore-errors'])) {
                        $this->rollbackFileTransaction();
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
                    $this->rollbackFileTransaction();
                    return $built;
                }
                foreach ($built as $ext) {
                    $bn = basename($ext['file']);
                    $this->log(2, "installing $bn");
                    $dest = $this->config->get('ext_dir') . DIRECTORY_SEPARATOR . $bn;
                    $this->log(3, "+ cp $ext[file] ext_dir");
                    $copyto = $this->_prependPath($dest, $this->installroot);
                    if (!@copy($ext['file'], $copyto)) {
                        $this->rollbackFileTransaction();
                        return $this->raiseError("failed to copy $bn to $copyto");
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

        if (!$this->commitFileTransaction()) {
            $this->rollbackFileTransaction();
            return $this->raiseError("commit failed", PEAR_INSTALLER_FAILED);
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

    function uninstall($package, $options = array())
    {
        $php_dir = $this->config->get('php_dir');
        if (isset($options['installroot'])) {
            if (substr($options['installroot'], -1) == DIRECTORY_SEPARATOR) {
                $options['installroot'] = substr($options['installroot'], 0, -1);
            }
            $this->installroot = $options['installroot'];
            $php_dir = $this->_prependPath($php_dir, $this->installroot);
        } else {
            $this->installroot = '';
        }
        $this->registry = &new PEAR_Registry($php_dir);

        // Delete the files
        if (PEAR::isError($err = $this->_deletePackageFiles($package))) {
            $this->rollbackFileTransaction();
            return $this->raiseError($err);
        }
        if (!$this->commitFileTransaction()) {
            $this->rollbackFileTransaction();
            return $this->raiseError("uninstall failed");
        }

        // Register that the package is no longer installed
        return $this->registry->deletePackage($package);
    }

    // }}}
    // {{{ checkDeps()

    function checkDeps(&$pkginfo)
    {
        $depchecker = &new PEAR_Dependency($this->registry);
        $error = $errors = '';
        $failed_deps = array();
        if (is_array($pkginfo['release_deps'])) {
            foreach($pkginfo['release_deps'] as $dep) {
                $code = $depchecker->callCheckMethod($error, $dep);
                if ($code) {
                    $failed_deps[] = array($dep, $code, $error);
                }
            }
            $n = count($failed_deps);
            if ($n > 0) {
                $depinstaller =& new PEAR_Installer($this->ui);
                $to_install = array();
                for ($i = 0; $i < $n; $i++) {
                    if (isset($failed_deps[$i]['type'])) {
                        $type = $failed_deps[$i]['type'];
                    } else {
                        $type = 'pkg';
                    }
                    switch ($failed_deps[$i][1]) {
                        case PEAR_DEPENDENCY_MISSING:
                            if ($type == 'pkg') {
                                // install
                            }
                            $errors .= "\n" . $failed_deps[$i][2];
                            break;
                        case PEAR_DEPENDENCY_UPGRADE_MINOR:
                            if ($type == 'pkg') {
                                // upgrade
                            }
                            $errors .= "\n" . $failed_deps[$i][2];
                            break;
                        default:
                            $errors .= "\n" . $failed_deps[$i][2];
                            break;
                    }
                }
                return substr($errors, 1);
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

if (!function_exists("md5_file")) {
    function md5_file($filename) {
        $fp = fopen($filename, "r");
        if (!$fp) return null;
        $contents = fread($fp, filesize($filename));
        fclose($fp);
        return md5($contents);
    }
}

?>
