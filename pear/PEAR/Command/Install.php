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
// | Author: Stig Sæther Bakken <ssb@fast.no>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR/Command/Common.php";
require_once "PEAR/Installer.php";
require_once "Console/Getopt.php";

/**
 * PEAR commands for installation or deinstallation/upgrading of
 * packages.
 *
 */
class PEAR_Command_Install extends PEAR_Command_Common
{
    // {{{ command definitions

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
                ),
            'doc' => '<package> ...
Uninstalls one or more PEAR packages.  More than one package may be
specified at once.
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

    function doInstall($command, $options, $params)
    {
        if (empty($this->installer)) {
            $this->installer = &new PEAR_Installer($this->ui);
        }
        if ($command == 'upgrade') {
            $options[$command] = true;
        }
        if ($command == 'upgrade-all') {
            include_once "PEAR/Remote.php";
            $options['upgrade'] = true;
            $remote = new PEAR_Remote($this->config);
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
                $this->ui->outputData("will upgrade $package", $command);
            }
        }
        foreach ($params as $pkg) {
            $bn = basename($pkg);
            $info = $this->installer->install($pkg, $options, $this->config);
            if (is_array($info)) {
                if ($this->config->get('verbose') > 0) {
                    $label = "$info[package] $info[version]";
                    $this->ui->outputData("$command ok: $label", $command);
                }
            } else {
                return $this->raiseError("$command failed");
            }
        }
        return true;
    }

    function doUninstall($command, $options, $params)
    {
        if (empty($this->installer)) {
            $this->installer = &new PEAR_Installer($this->ui);
        }
        if (sizeof($params) < 1) {
            return $this->raiseError("Please supply the package(s) you want to uninstall");
        }
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

}

?>