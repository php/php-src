<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Stig Sæther Bakken <ssb@php.net>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR/Command/Common.php";
require_once "PEAR/Installer.php";

/**
 * PEAR commands for installation or deinstallation/upgrading of
 * packages.
 *
 */
class PEAR_Command_Install extends PEAR_Command_Common
{
    // {{{ properties

    var $commands = array(
        'install' => array(
            'summary' => 'Install Package',
            'function' => 'doInstall',
            'shortcut' => 'i',
            'options' => array(
                'force' => array(
                    'shortopt' => 'f',
                    'doc' => 'will overwrite newer installed packages',
                    ),
                'nodeps' => array(
                    'shortopt' => 'n',
                    'doc' => 'ignore dependencies, install anyway',
                    ),
                'register-only' => array(
                    'shortopt' => 'r',
                    'doc' => 'do not install files, only register the package as installed',
                    ),
                'soft' => array(
                    'shortopt' => 's',
                    'doc' => 'soft install, fail silently, or upgrade if already installed',
                    ),
                'nobuild' => array(
                    'shortopt' => 'B',
                    'doc' => 'don\'t build C extensions',
                    ),
                'nocompress' => array(
                    'shortopt' => 'Z',
                    'doc' => 'request uncompressed files when downloading',
                    ),
                'installroot' => array(
                    'shortopt' => 'R',
                    'arg' => 'DIR',
                    'doc' => 'root directory used when installing files (ala PHP\'s INSTALL_ROOT)',
                    ),
                'ignore-errors' => array(
                    'doc' => 'force install even if there were errors',
                    ),
                'alldeps' => array(
                    'shortopt' => 'a',
                    'doc' => 'install all required and optional dependencies',
                    ),
                'onlyreqdeps' => array(
                    'shortopt' => 'o',
                    'doc' => 'install all required dependencies',
                    ),
                ),
            'doc' => '<package> ...
Installs one or more PEAR packages.  You can specify a package to
install in four ways:

"Package-1.0.tgz" : installs from a local file

"http://example.com/Package-1.0.tgz" : installs from
anywhere on the net.

"package.xml" : installs the package described in
package.xml.  Useful for testing, or for wrapping a PEAR package in
another package manager such as RPM.

"Package" : queries your configured server
({config master_server}) and downloads the newest package with
the preferred quality/state ({config preferred_state}).

More than one package may be specified at once.  It is ok to mix these
four ways of specifying packages.
'),
        'upgrade' => array(
            'summary' => 'Upgrade Package',
            'function' => 'doInstall',
            'shortcut' => 'up',
            'options' => array(
                'force' => array(
                    'shortopt' => 'f',
                    'doc' => 'overwrite newer installed packages',
                    ),
                'nodeps' => array(
                    'shortopt' => 'n',
                    'doc' => 'ignore dependencies, upgrade anyway',
                    ),
                'register-only' => array(
                    'shortopt' => 'r',
                    'doc' => 'do not install files, only register the package as upgraded',
                    ),
                'nobuild' => array(
                    'shortopt' => 'B',
                    'doc' => 'don\'t build C extensions',
                    ),
                'nocompress' => array(
                    'shortopt' => 'Z',
                    'doc' => 'request uncompressed files when downloading',
                    ),
                'installroot' => array(
                    'shortopt' => 'R',
                    'arg' => 'DIR',
                    'doc' => 'root directory used when installing files (ala PHP\'s INSTALL_ROOT)',
                    ),
                'ignore-errors' => array(
                    'doc' => 'force install even if there were errors',
                    ),
                'alldeps' => array(
                    'shortopt' => 'a',
                    'doc' => 'install all required and optional dependencies',
                    ),
                'onlyreqdeps' => array(
                    'shortopt' => 'o',
                    'doc' => 'install all required dependencies',
                    ),
                ),
            'doc' => '<package> ...
Upgrades one or more PEAR packages.  See documentation for the
"install" command for ways to specify a package.

When upgrading, your package will be updated if the provided new
package has a higher version number (use the -f option if you need to
upgrade anyway).

More than one package may be specified at once.
'),
        'upgrade-all' => array(
            'summary' => 'Upgrade All Packages',
            'function' => 'doInstall',
            'shortcut' => 'ua',
            'options' => array(
                'nodeps' => array(
                    'shortopt' => 'n',
                    'doc' => 'ignore dependencies, upgrade anyway',
                    ),
                'register-only' => array(
                    'shortopt' => 'r',
                    'doc' => 'do not install files, only register the package as upgraded',
                    ),
                'nobuild' => array(
                    'shortopt' => 'B',
                    'doc' => 'don\'t build C extensions',
                    ),
                'nocompress' => array(
                    'shortopt' => 'Z',
                    'doc' => 'request uncompressed files when downloading',
                    ),
                'installroot' => array(
                    'shortopt' => 'R',
                    'arg' => 'DIR',
                    'doc' => 'root directory used when installing files (ala PHP\'s INSTALL_ROOT)',
                    ),
                'ignore-errors' => array(
                    'doc' => 'force install even if there were errors',
                    ),
                ),
            'doc' => '
Upgrades all packages that have a newer release available.  Upgrades are
done only if there is a release available of the state specified in
"preferred_state" (currently {config preferred_state}), or a state considered
more stable.
'),
        'uninstall' => array(
            'summary' => 'Un-install Package',
            'function' => 'doUninstall',
            'shortcut' => 'un',
            'options' => array(
                'nodeps' => array(
                    'shortopt' => 'n',
                    'doc' => 'ignore dependencies, uninstall anyway',
                    ),
                'register-only' => array(
                    'shortopt' => 'r',
                    'doc' => 'do not remove files, only register the packages as not installed',
                    ),
                'installroot' => array(
                    'shortopt' => 'R',
                    'arg' => 'DIR',
                    'doc' => 'root directory used when installing files (ala PHP\'s INSTALL_ROOT)',
                    ),
                'ignore-errors' => array(
                    'doc' => 'force install even if there were errors',
                    ),
                ),
            'doc' => '<package> ...
Uninstalls one or more PEAR packages.  More than one package may be
specified at once.
'),
        'bundle' => array(
            'summary' => 'Unpacks a Pecl Package',
            'function' => 'doBundle',
            'shortcut' => 'bun',
            'options' => array(
                'destination' => array(
                   'shortopt' => 'd',
                    'arg' => 'DIR',
                    'doc' => 'Optional destination directory for unpacking (defaults to current path or "ext" if exists)',
                    ),
                'force' => array(
                    'shortopt' => 'f',
                    'doc' => 'Force the unpacking even if there were errors in the package',
                ),
            ),
            'doc' => '<package>
Unpacks a Pecl Package into the selected location. It will download the
package if needed.
'),
    );

    // }}}
    // {{{ constructor

    /**
     * PEAR_Command_Install constructor.
     *
     * @access public
     */
    function PEAR_Command_Install(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ doInstall()

    function doInstall($command, $options, $params)
    {
        require_once 'PEAR/Downloader.php';
        if (empty($this->installer)) {
            $this->installer = &new PEAR_Installer($this->ui);
        }
        if ($command == 'upgrade') {
            $options['upgrade'] = true;
        }
        if ($command == 'upgrade-all') {
            include_once "PEAR/Remote.php";
            $options['upgrade'] = true;
            $remote = &new PEAR_Remote($this->config);
            $state = $this->config->get('preferred_state');
            if (empty($state) || $state == 'any') {
                $latest = $remote->call("package.listLatestReleases");
            } else {
                $latest = $remote->call("package.listLatestReleases", $state);
            }
            if (PEAR::isError($latest)) {
                return $latest;
            }
            $reg = new PEAR_Registry($this->config->get('php_dir'));
            $installed = array_flip($reg->listPackages());
            $params = array();
            foreach ($latest as $package => $info) {
                $package = strtolower($package);
                if (!isset($installed[$package])) {
                    // skip packages we don't have installed
                    continue;
                }
                $inst_version = $reg->packageInfo($package, 'version');
                if (version_compare("$info[version]", "$inst_version", "le")) {
                    // installed version is up-to-date
                    continue;
                }
                $params[] = $package;
                $this->ui->outputData(array('data' => "Will upgrade $package"), $command);
            }
        }
        $this->downloader = &new PEAR_Downloader($this->ui, $options, $this->config);
        $errors = array();
        $downloaded = array();
        $this->downloader->download($params);
        $errors = $this->downloader->getErrorMsgs();
        if (count($errors)) {
            $err['data'] = array($errors);
            $err['headline'] = 'Install Errors';
            $this->ui->outputData($err);
            return $this->raiseError("$command failed");
        }
        $downloaded = $this->downloader->getDownloadedPackages();
        $this->installer->sortPkgDeps($downloaded);
        foreach ($downloaded as $pkg) {
            PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
            $info = $this->installer->install($pkg['file'], $options, $this->config);
            PEAR::popErrorHandling();
            if (PEAR::isError($info)) {
                $this->ui->outputData('ERROR: ' .$info->getMessage());
                continue;
            }
            if (is_array($info)) {
                if ($this->config->get('verbose') > 0) {
                    $label = "$info[package] $info[version]";
                    $out = array('data' => "$command ok: $label");
                    if (isset($info['release_warnings'])) {
                        $out['release_warnings'] = $info['release_warnings'];
                    }
                    $this->ui->outputData($out, $command);
                }
            } else {
                return $this->raiseError("$command failed");
            }
        }
        return true;
    }

    // }}}
    // {{{ doUninstall()

    function doUninstall($command, $options, $params)
    {
        if (empty($this->installer)) {
            $this->installer = &new PEAR_Installer($this->ui);
        }
        if (sizeof($params) < 1) {
            return $this->raiseError("Please supply the package(s) you want to uninstall");
        }
        include_once 'PEAR/Registry.php';
        $reg = new PEAR_Registry($this->config->get('php_dir'));
        $newparams = array();
        $badparams = array();
        foreach ($params as $pkg) {
            $info = $reg->packageInfo($pkg);
            if ($info === null) {
                $badparams[] = $pkg;
            } else {
                $newparams[] = $info;
            }
        }
        $this->installer->sortPkgDeps($newparams, true);
        $params = array();
        foreach($newparams as $info) {
            $params[] = $info['info']['package'];
        }
        $params = array_merge($params, $badparams);
        foreach ($params as $pkg) {
            if ($this->installer->uninstall($pkg, $options)) {
                if ($this->config->get('verbose') > 0) {
                    $this->ui->outputData("uninstall ok: $pkg", $command);
                }
            } else {
                return $this->raiseError("uninstall failed: $pkg");
            }
        }
        return true;
    }

    // }}}


    // }}}
    // {{{ doBundle()
    /*
    (cox) It just downloads and untars the package, does not do
            any check that the PEAR_Installer::_installFile() does.
    */

    function doBundle($command, $options, $params)
    {
        if (empty($this->installer)) {
            $this->installer = &new PEAR_Downloader($this->ui);
        }
        $installer = &$this->installer;
        if (sizeof($params) < 1) {
            return $this->raiseError("Please supply the package you want to bundle");
        }
        $pkgfile = $params[0];
        $need_download = false;
        if (preg_match('#^(http|ftp)://#', $pkgfile)) {
            $need_download = true;
        } elseif (!@is_file($pkgfile)) {
            if ($installer->validPackageName($pkgfile)) {
                $pkgfile = $installer->getPackageDownloadUrl($pkgfile);
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
            $downloaddir = $installer->config->get('download_dir');
            if (empty($downloaddir)) {
                if (PEAR::isError($downloaddir = System::mktemp('-d'))) {
                    return $downloaddir;
                }
                $installer->log(2, '+ tmp dir created at ' . $downloaddir);
            }
            $callback = $this->ui ? array(&$installer, '_downloadCallback') : null;
            $file = $installer->downloadHttp($pkgfile, $this->ui, $downloaddir, $callback);
            if (PEAR::isError($file)) {
                return $this->raiseError($file);
            }
            $pkgfile = $file;
        }

       // Parse xml file -----------------------------------------------
        $pkginfo = $installer->infoFromTgzFile($pkgfile);
        if (PEAR::isError($pkginfo)) {
            return $this->raiseError($pkginfo);
        }
        $installer->validatePackageInfo($pkginfo, $errors, $warnings);
        // XXX We allow warnings, do we have to do it?
        if (count($errors)) {
             if (empty($options['force'])) {
                return $this->raiseError("The following errors where found:\n".
                                                 implode("\n", $errors));
            } else {
                $this->log(0, "warning : the following errors were found:\n".
                           implode("\n", $errors));
            }
        }
        $pkgname = $pkginfo['package'];

        // Unpacking -------------------------------------------------

        if (isset($options['destination'])) {
            if (!is_dir($options['destination'])) {
                System::mkdir('-p ' . $options['destination']);
            }
            $dest = realpath($options['destination']);
        } else {
            $pwd = getcwd();
            if (is_dir($pwd . DIRECTORY_SEPARATOR . 'ext')) {
                $dest = $pwd . DIRECTORY_SEPARATOR . 'ext';
            } else {
                $dest = $pwd;
            }
        }
        $dest .= DIRECTORY_SEPARATOR . $pkgname;
        $orig = $pkgname . '-' . $pkginfo['version'];

        $tar = new Archive_Tar($pkgfile);
        if (!@$tar->extractModify($dest, $orig)) {
            return $this->raiseError("unable to unpack $pkgfile");
        }
        $this->ui->outputData("Package ready at '$dest'");
    // }}}
    }

    // }}}

}
?>
