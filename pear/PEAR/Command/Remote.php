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

    // {{{ getCommands()

    /**
     * Return a list of all the commands defined by this class.
     * @return array list of commands
     * @access public
     */
    function getCommands()
    {
        return array('remote-package-info',
                     'list-upgrades',
                     'list-remote-packages',
                     'download');
    }

    // }}}
    // {{{ run()

    /**
     * Execute the command.
     *
     * @param string command name
     *
     * @param array option_name => value
     *
     * @param array list of additional parameters
     *
     * @return bool TRUE on success, FALSE for unknown commands, or
     * a PEAR error on failure
     *
     * @access public
     */
    function run($command, $options, $params)
    {
        $failmsg = '';
        $remote = &new PEAR_Remote($this->config);
        switch ($command) {
            // {{{ remote-package-info

            case 'remote-package-info': {
                break;
            }

            // }}}
            // {{{ list-remote-packages

            case 'list-remote-packages': {
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
                break;
            }

            // }}}
            // {{{ download

            case 'download': {
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
                break;
            }

            // }}}
            // {{{ list-upgrades

            case 'list-upgrades': {
                include_once "PEAR/Registry.php";
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
                break;
            }

            // }}}
            default: {
                return false;
            }
        }
        if ($failmsg) {
            return $this->raiseError($failmsg);
        }
        return true;
    }

    // }}}

    function downloadCallback($msg, $params = null)
    {
        if ($msg == 'done') {
            $this->bytes_downloaded = $params;
        }
    }
}

?>