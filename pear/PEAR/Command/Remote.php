<?php
// /* vim: set expandtab tabstop=4 shiftwidth=4: */
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
// | Author: Stig Bakken <ssb@fast.no>                                    |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Common.php';
require_once 'PEAR/Remote.php';

class PEAR_Command_Remote extends PEAR_Command_Common
{
    // {{{ command definitions

    var $commands = array(
        'remote-package-info' => array(
            'summary' => 'Information About Remote Packages',
            'function' => 'doRemotePackageInfo',
            'options' => array(),
            ),
        'list-upgrades' => array(
            'summary' => 'List Available Upgrades',
            'function' => 'doListUpgrades',
            'options' => array(),
            ),
        'list-remote-packages' => array(
            'summary' => 'List Remote Packages',
            'function' => 'doListRemotePackages',
            'options' => array(),
            ),
        'download' => array(
            'summary' => 'Download Package',
            'function' => 'doDownload',
            'options' => array(),
            ),
        );

    // }}}
    // {{{ constructor

    /**
     * PEAR_Command_Remote constructor.
     *
     * @access public
     */
    function PEAR_Command_Remote(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ remote-package-info

    function doRemotePackageInfo($command, $options, $params)
    {
    }

    // }}}
    // {{{ list-remote-packages

    function doListRemotePackages($command, $options, $params)
    {
        $r = new PEAR_Remote($this->config);
        $available = $r->call('package.listAll', true);
        if (PEAR::isError($available)) {
            return $this->raiseError($available);
        }
        $i = $j = 0;
        $this->ui->startTable(
            array('caption' => 'Available packages:',
                  'border' => true));
        foreach ($available as $name => $info) {
            if ($i++ % 20 == 0) {
                $this->ui->tableRow(
                    array('Package', 'Version'),
                    array('bold' => true));
            }
            $this->ui->tableRow(array($name, $info['stable']));
        }
        if ($i == 0) {
            $this->ui->tableRow(array('(no packages installed yet)'));
        }
        $this->ui->endTable();
        return true;
    }

    // }}}
    // {{{ download

    function doDownload($command, $options, $params)
    {
        //$params[0] -> The package to download
        if (count($params) != 1) {
            return PEAR::raiseError("download expects one argument: the package to download");
        }
        $server = $this->config->get('master_server');
        if (!ereg('^http://', $params[0])) {
            $pkgfile = "http://$server/get/$params[0]";
        } else {
            $pkgfile = $params[0];
        }
        $this->bytes_downloaded = 0;
        $saved = PEAR_Common::downloadHttp($pkgfile, $this->ui, '.',
                                           array(&$this, 'downloadCallback'));
        if (PEAR::isError($saved)) {
            return $this->raiseError($saved);
        }
        $fname = basename($saved);
        $this->ui->displayLine("File $fname downloaded ($this->bytes_downloaded bytes)");
        return true;
    }

    function downloadCallback($msg, $params = null)
    {
        if ($msg == 'done') {
            $this->bytes_downloaded = $params;
        }
    }

    // }}}
    // {{{ list-upgrades

    function doListUpgrades($command, $options, $params)
    {
        include_once "PEAR/Registry.php";
        $remote = new PEAR_Remote($this->config);
        if (empty($params[0])) {
            $state = $this->config->get('preferred_state');
        } else {
            $state = $params[0];
        }
        $caption = 'Available Upgrades';
        if (empty($state) || $state == 'any') {
            $latest = $remote->call("package.listLatestReleases");
        } else {
            $latest = $remote->call("package.listLatestReleases", $state);
            $caption .= ' (' . $state . ')';
        }
        $caption .= ':';
        if (PEAR::isError($latest)) {
            return $latest;
        }
        $reg = new PEAR_Registry($this->config->get('php_dir'));
        $inst = array_flip($reg->listPackages());
        $this->ui->startTable(array('caption' => $caption,
                                    'border' => 1));
        $this->ui->tableRow(array('Package', 'Version', 'Size'),
                            array('bold' => true));
        foreach ($latest as $package => $info) {
            if (!isset($inst[$package])) {
                // skip packages we don't have installed
                continue;
            }
            extract($info);
            $inst_version = $reg->packageInfo($package, 'version');
            if (version_compare($version, $inst_version, "le")) {
                // installed version is up-to-date
                continue;
            }
            if ($filesize >= 20480) {
                $filesize += 1024 - ($filesize % 1024);
                $fs = sprintf("%dkB", $filesize / 1024);
            } elseif ($filesize > 0) {
                $filesize += 103 - ($filesize % 103);
                $fs = sprintf("%.1fkB", $filesize / 1024.0);
            } else {
                $fs = "  -"; // XXX center instead
            }
            $this->ui->tableRow(array($package, $version, $fs));
        }
        $this->ui->endTable();
        return true;
    }

    // }}}
}

?>