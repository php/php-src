<?php
// /* vim: set expandtab tabstop=4 shiftwidth=4: */
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
// | Author: Stig Bakken <ssb@fast.no>                                    |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Command/Common.php';
require_once 'PEAR/Common.php';
require_once 'PEAR/Remote.php';
require_once 'PEAR/Registry.php';

class PEAR_Command_Remote extends PEAR_Command_Common
{
    // {{{ command definitions

    var $commands = array(
        'remote-info' => array(
            'summary' => 'Information About Remote Packages',
            'function' => 'doRemoteInfo',
            'shortcut' => 'ri',
            'options' => array(),
            'doc' => '<package>
Get details on a package from the server.',
            ),
        'list-upgrades' => array(
            'summary' => 'List Available Upgrades',
            'function' => 'doListUpgrades',
            'shortcut' => 'lu',
            'options' => array(),
            'doc' => '
List releases on the server of packages you have installed where
a newer version is available with the same release state (stable etc.).'
            ),
        'remote-list' => array(
            'summary' => 'List Remote Packages',
            'function' => 'doRemoteList',
            'shortcut' => 'rl',
            'options' => array(),
            'doc' => '
Lists the packages available on the configured server along with the
latest stable release of each package.',
            ),
        'search' => array(
            'summary' => 'Search remote package database',
            'function' => 'doSearch',
            'shortcut' => 'sp',
            'options' => array(),
            'doc' => '
Lists all packages which match the search paramteres (first param
is package name, second package info)',
            ),
        'list-all' => array(
            'summary' => 'List All Packages',
            'function' => 'doListAll',
            'shortcut' => 'la',
            'options' => array(),
            'doc' => '
Lists the packages available on the configured server along with the
latest stable release of each package.',
            ),
        'download' => array(
            'summary' => 'Download Package',
            'function' => 'doDownload',
            'shortcut' => 'd',
            'options' => array(
                'nocompress' => array(
                    'shortopt' => 'Z',
                    'doc' => 'download an uncompressed (.tar) file',
                    ),
                ),
            'doc' => '{package|package-version}
Download a package tarball.  The file will be named as suggested by the
server, for example if you download the DB package and the latest stable
version of DB is 1.2, the downloaded file will be DB-1.2.tgz.',
            ),
        'clear-cache' => array(
            'summary' => 'Clear XML-RPC Cache',
            'function' => 'doClearCache',
            'shortcut' => 'cc',
            'options' => array(),
            'doc' => '
Clear the XML-RPC cache.  See also the cache_ttl configuration
parameter.
',
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

    // {{{ doRemoteInfo()

    function doRemoteInfo($command, $options, $params)
    {
        if (sizeof($params) != 1) {
            return $this->raiseError("$command expects one param: the remote package name");
        }
        $r = new PEAR_Remote($this->config);
        $info = $r->call('package.info', $params[0]);
        if (PEAR::isError($info)) {
            return $this->raiseError($info);
        }

        $reg = new PEAR_Registry($this->config->get('php_dir'));
        $installed = $reg->packageInfo($info['name']);
        $info['installed'] = $installed['version'] ? $installed['version'] : '- no -';

        $this->ui->outputData($info, $command);

        return true;
    }

    // }}}
    // {{{ doRemoteList()

    function doRemoteList($command, $options, $params)
    {
        $r = new PEAR_Remote($this->config);
        $list_options = false;
        if ($this->config->get('preferred_state') == 'stable')
            $list_options = true;
        $available = $r->call('package.listAll', $list_options);
        if (PEAR::isError($available)) {
            return $this->raiseError($available);
        }
        $i = $j = 0;
        $data = array(
            'caption' => 'Available packages:',
            'border' => true,
            'headline' => array('Package', 'Version'),
            );
        foreach ($available as $name => $info) {
            $data['data'][] = array($name, isset($info['stable']) ? $info['stable'] : '-n/a-');
        }
        if (count($available)==0) {
            $data = '(no packages installed yet)';
        }
        $this->ui->outputData($data, $command);
        return true;
    }

    // }}}
    // {{{ doListAll()

    function doListAll($command, $options, $params)
    {
        $r = new PEAR_Remote($this->config);
        $reg = new PEAR_Registry($this->config->get('php_dir'));
        $list_options = false;
        if ($this->config->get('preferred_state') == 'stable')
            $list_options = true;
        $available = $r->call('package.listAll', $list_options);
        if (PEAR::isError($available)) {
            return $this->raiseError($available);
        }
        if (!is_array($available)) {
            return $this->raiseError('The package list could not be fetched from the remote server. Please try again. (Debug info: "'.$available.'")');
        }
        $data = array(
            'caption' => 'All packages:',
            'border' => true,
            'headline' => array('Package', 'Latest', 'Local'),
            );

        foreach ($available as $name => $info) {
            $installed = $reg->packageInfo($name);
            $desc = $info['summary'];
            if (isset($params[$name]))
                $desc .= "\n\n".$info['description'];

            if (isset($options['mode']))
            {
                if ($options['mode'] == 'installed' && !isset($installed['version']))
                    continue;
                if ($options['mode'] == 'notinstalled' && isset($installed['version']))
                    continue;
                if ($options['mode'] == 'upgrades'
                    && (!isset($installed['version']) || $installed['version'] == $info['stable']))
                {
                    continue;
                };
            };

            $data['data'][$info['category']][] = array(
                $name,
                @$info['stable'],
                @$installed['version'],
                @$desc,
                @$info['deps'],
                );
        }
        $this->ui->outputData($data, $command);
        return true;
    }

    // }}}
    // {{{ doSearch()

    function doSearch($command, $options, $params)
    {
        if ((!isset($params[0]) || empty($params[0]))
            && (!isset($params[1]) || empty($params[1])))
        {
            return $this->raiseError('no valid search string supplied');
        };

        $r = new PEAR_Remote($this->config);
        $reg = new PEAR_Registry($this->config->get('php_dir'));
        $available = $r->call('package.listAll', true);
        if (PEAR::isError($available)) {
            return $this->raiseError($available);
        }
        $data = array(
            'caption' => 'Matched packages:',
            'border' => true,
            'headline' => array('Package', 'Latest', 'Local'),
            );

        foreach ($available as $name => $info) {
            $found = (!empty($params[0]) && stristr($name, $params[0]) !== false);
            if (!$found && !(isset($params[1]) && !empty($params[1])
                && (stristr($info['summary'], $params[1]) !== false
                    || stristr($info['description'], $params[1]) !== false)))
            {
                continue;
            };

            $installed = $reg->packageInfo($name);
            $desc = $info['summary'];
            if (isset($params[$name]))
                $desc .= "\n\n".$info['description'];

            $data['data'][$info['category']][] = array(
                $name,
                $info['stable'],
                $installed['version'],
                $desc,
                );
        }
        if (!isset($data['data'])) {
            return $this->raiseError('no packages found');
        };
        $this->ui->outputData($data, $command);
        return true;
    }

    // }}}
    // {{{ doDownload()

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
        $this->ui->outputData("File $fname downloaded ($this->bytes_downloaded bytes)", $command);
        return true;
    }

    function downloadCallback($msg, $params = null)
    {
        if ($msg == 'done') {
            $this->bytes_downloaded = $params;
        }
    }

    // }}}
    // {{{ doListUpgrades()

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
        $data = array(
            'caption' => $caption,
            'border' => 1,
            'headline' => array('Package', 'Version', 'Size'),
            );
        foreach ($latest as $package => $info) {
            if (!isset($inst[$package])) {
                // skip packages we don't have installed
                continue;
            }
            extract($info);
            $inst_version = $reg->packageInfo($package, 'version');
            if (version_compare("$version", "$inst_version", "le")) {
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
            $data['data'][] = array($package, $version, $fs);
        }
        if (empty($data['data'])) {
            $this->ui->outputData('No upgrades available');
        } else {
            $this->ui->outputData($data, $command);
        }
        return true;
    }

    // }}}
    // {{{ doClearCache()

    function doClearCache($command, $options, $params)
    {
        $cache_dir = $this->config->get('cache_dir');
        $verbose = $this->config->get('verbose');
        $output = '';
        if (!($dp = @opendir($cache_dir))) {
            return $this->raiseError("opendir($cache_dir) failed: $php_errormsg");
        }
        if ($verbose >= 1) {
            $output .= "reading directory $cache_dir\n";
        }
        $num = 0;
        while ($ent = readdir($dp)) {
            if (preg_match('/^xmlrpc_cache_[a-z0-9]{32}$/', $ent)) {
                $path = $cache_dir . DIRECTORY_SEPARATOR . $ent;
                $ok = @unlink($path);
                if ($ok) {
                    if ($verbose >= 2) {
                        $output .= "deleted $path\n";
                    }
                    $num++;
                } elseif ($verbose >= 1) {
                    $output .= "failed to delete $path\n";
                }
            }
        }
        closedir($dp);
        if ($verbose >= 1) {
            $output .= "$num cache entries cleared\n";
        }
        $this->ui->outputData(rtrim($output), $command);
        return $num;
    }

    // }}}
}

?>
