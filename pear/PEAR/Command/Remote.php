<?php
/**
 * PEAR_Command_Remote (remote-info, list-upgrades, remote-list, search, list-all, download,
 * clear-cache commands)
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * base class
 */
require_once 'PEAR/Command/Common.php';
require_once 'PEAR/REST.php';

/**
 * PEAR commands for remote server querying
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
 */
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
            'doc' => '[preferred_state]
List releases on the server of packages you have installed where
a newer version is available with the same release state (stable etc.)
or the state passed as the second parameter.'
            ),
        'remote-list' => array(
            'summary' => 'List Remote Packages',
            'function' => 'doRemoteList',
            'shortcut' => 'rl',
            'options' => array(
                'channel' =>
                    array(
                    'shortopt' => 'c',
                    'doc' => 'specify a channel other than the default channel',
                    'arg' => 'CHAN',
                    )
                ),
            'doc' => '
Lists the packages available on the configured server along with the
latest stable release of each package.',
            ),
        'search' => array(
            'summary' => 'Search remote package database',
            'function' => 'doSearch',
            'shortcut' => 'sp',
            'options' => array(
                'channel' =>
                    array(
                    'shortopt' => 'c',
                    'doc' => 'specify a channel other than the default channel',
                    'arg' => 'CHAN',
                    )
                ),
            'doc' => '[packagename] [packageinfo]
Lists all packages which match the search parameters.  The first
parameter is a fragment of a packagename.  The default channel
will be used unless explicitly overridden.  The second parameter
will be used to match any portion of the summary/description',
            ),
        'list-all' => array(
            'summary' => 'List All Packages',
            'function' => 'doListAll',
            'shortcut' => 'la',
            'options' => array(
                'channel' =>
                    array(
                    'shortopt' => 'c',
                    'doc' => 'specify a channel other than the default channel',
                    'arg' => 'CHAN',
                    )
                ),
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
            'doc' => '<package>...
Download package tarballs.  The files will be named as suggested by the
server, for example if you download the DB package and the latest stable
version of DB is 1.6.5, the downloaded file will be DB-1.6.5.tgz.',
            ),
        'clear-cache' => array(
            'summary' => 'Clear Web Services Cache',
            'function' => 'doClearCache',
            'shortcut' => 'cc',
            'options' => array(),
            'doc' => '
Clear the XML-RPC/REST cache.  See also the cache_ttl configuration
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

    function _checkChannelForStatus($channel, $chan)
    {
        $rest = new PEAR_REST($this->config);
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $a = $rest->downloadHttp('http://' . $channel .
            '/channel.xml', $chan->lastModified());
        PEAR::staticPopErrorHandling();
        if (!PEAR::isError($a) && $a) {
            $this->ouputData('WARNING: channel "' . $channel . '" has ' .
                'updated its protocols, use "channel-update ' . $channel .
                '" to update');
        }
    }

    // {{{ doRemoteInfo()

    function doRemoteInfo($command, $options, $params)
    {
        if (sizeof($params) != 1) {
            return $this->raiseError("$command expects one param: the remote package name");
        }
        $savechannel = $channel = $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        $package = $params[0];
        $parsed = $reg->parsePackageName($package, $channel);
        if (PEAR::isError($parsed)) {
            return $this->raiseError('Invalid package name "' . $package . '"');
        }
        
        $channel = $parsed['channel'];
        $this->config->set('default_channel', $channel);
        $chan = $reg->getChannel($channel);
        $this->_checkChannelForStatus($channel, $chan);
        if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
            $rest = &$this->config->getREST('1.0', array());
            $info = $rest->packageInfo($base, $parsed['package']);
        } else {
            $r = &$this->config->getRemote();
            $info = $r->call('package.info', $parsed['package']);
        }
        if (PEAR::isError($info)) {
            $this->config->set('default_channel', $savechannel);
            return $this->raiseError($info);
        }
        if (!isset($info['name'])) {
            return $this->raiseError('No remote package "' . $package . '" was found');
        }

        $installed = $reg->packageInfo($info['name'], null, $channel);
        $info['installed'] = $installed['version'] ? $installed['version'] : '- no -';
        if (is_array($info['installed'])) {
            $info['installed'] = $info['installed']['release'];
        }

        $this->ui->outputData($info, $command);
        $this->config->set('default_channel', $savechannel);

        return true;
    }

    // }}}
    // {{{ doRemoteList()

    function doRemoteList($command, $options, $params)
    {
        $savechannel = $channel = $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        if (isset($options['channel'])) {
            $channel = $options['channel'];
            if ($reg->channelExists($channel)) {
                $this->config->set('default_channel', $channel);
            } else {
                return $this->raiseError('Channel "' . $channel . '" does not exist');
            }
        }
        $chan = $reg->getChannel($channel);
        $this->_checkChannelForStatus($channel, $chan);
        $list_options = false;
        if ($this->config->get('preferred_state') == 'stable') {
            $list_options = true;
        }
        if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.1', $this->config->get('preferred_mirror'))) {
            // use faster list-all if available
            $rest = &$this->config->getREST('1.1', array());
            $available = $rest->listAll($base, $list_options);
        } elseif ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
            $rest = &$this->config->getREST('1.0', array());
            $available = $rest->listAll($base, $list_options);
        } else {
            $r = &$this->config->getRemote();
            if ($channel == 'pear.php.net') {
                // hack because of poor pearweb design
                $available = $r->call('package.listAll', true, $list_options, false);
            } else {
                $available = $r->call('package.listAll', true, $list_options);
            }
        }
        if (PEAR::isError($available)) {
            $this->config->set('default_channel', $savechannel);
            return $this->raiseError($available);
        }
        $i = $j = 0;
        $data = array(
            'caption' => 'Channel ' . $channel . ' Available packages:',
            'border' => true,
            'headline' => array('Package', 'Version'),
            );
        if (count($available)==0) {
            $data = '(no packages available yet)';
        } else {
            foreach ($available as $name => $info) {
                $data['data'][] = array($name, (isset($info['stable']) && $info['stable'])
                    ? $info['stable'] : '-n/a-');
            }
        }
        $this->ui->outputData($data, $command);
        $this->config->set('default_channel', $savechannel);
        return true;
    }

    // }}}
    // {{{ doListAll()

    function doListAll($command, $options, $params)
    {
        $savechannel = $channel = $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        if (isset($options['channel'])) {
            $channel = $options['channel'];
            if ($reg->channelExists($channel)) {
                $this->config->set('default_channel', $channel);
            } else {
                return $this->raiseError("Channel \"$channel\" does not exist");
            }
        }
        $list_options = false;
        if ($this->config->get('preferred_state') == 'stable') {
            $list_options = true;
        }
        $chan = $reg->getChannel($channel);
        $this->_checkChannelForStatus($channel, $chan);
        if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.1', $this->config->get('preferred_mirror'))) {
            // use faster list-all if available
            $rest = &$this->config->getREST('1.1', array());
            $available = $rest->listAll($base, $list_options, false);
        } elseif ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
            $rest = &$this->config->getREST('1.0', array());
            $available = $rest->listAll($base, $list_options, false);
        } else {
            $r = &$this->config->getRemote();
            if ($channel == 'pear.php.net') {
                // hack because of poor pearweb design
                $available = $r->call('package.listAll', true, $list_options, false);
            } else {
                $available = $r->call('package.listAll', true, $list_options);
            }
        }
        if (PEAR::isError($available)) {
            $this->config->set('default_channel', $savechannel);
            return $this->raiseError('The package list could not be fetched from the remote server. Please try again. (Debug info: "' . $available->getMessage() . '")');
        }
        $data = array(
            'caption' => 'All packages:',
            'border' => true,
            'headline' => array('Package', 'Latest', 'Local'),
            );
        $local_pkgs = $reg->listPackages($channel);

        foreach ($available as $name => $info) {
            $installed = $reg->packageInfo($name, null, $channel);
            if (is_array($installed['version'])) {
                $installed['version'] = $installed['version']['release'];
            }
            $desc = $info['summary'];
            if (isset($params[$name])) {
                $desc .= "\n\n".$info['description'];
            }
            if (isset($options['mode']))
            {
                if ($options['mode'] == 'installed' && !isset($installed['version'])) {
                    continue;
                }
                if ($options['mode'] == 'notinstalled' && isset($installed['version'])) {
                    continue;
                }
                if ($options['mode'] == 'upgrades'
                      && (!isset($installed['version']) || version_compare($installed['version'],
                      $info['stable'], '>='))) {
                    continue;
                }
            }
            $pos = array_search(strtolower($name), $local_pkgs);
            if ($pos !== false) {
                unset($local_pkgs[$pos]);
            }

            if (isset($info['stable']) && !$info['stable']) {
                $info['stable'] = null;
            }
            $data['data'][$info['category']][] = array(
                $reg->channelAlias($channel) . '/' . $name,
                @$info['stable'],
                @$installed['version'],
                @$desc,
                @$info['deps'],
                );
        }

        if (isset($options['mode']) && in_array($options['mode'], array('notinstalled', 'upgrades'))) {
            $this->config->set('default_channel', $savechannel);
            $this->ui->outputData($data, $command);
            return true;
        }
        foreach ($local_pkgs as $name) {
            $info = &$reg->getPackage($name, $channel);
            $data['data']['Local'][] = array(
                $reg->channelAlias($channel) . '/' . $info->getPackage(),
                '',
                $info->getVersion(),
                $info->getSummary(),
                $info->getDeps()
                );
        }

        $this->config->set('default_channel', $savechannel);
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

        $savechannel = $channel = $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        $package = $params[0];
        $summary = isset($params[1]) ? $params[1] : false;
        if (isset($options['channel'])) {
            $reg = &$this->config->getRegistry();
            $channel = $options['channel'];
            if ($reg->channelExists($channel)) {
                $this->config->set('default_channel', $channel);
            } else {
                return $this->raiseError('Channel "' . $channel . '" does not exist');
            }
        }
        $chan = $reg->getChannel($channel);
        $this->_checkChannelForStatus($channel, $chan);
        if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
            $rest = &$this->config->getREST('1.0', array());
            $available = $rest->listAll($base, false, false, $package, $summary);
        } else {
            $r = &$this->config->getRemote();
            $available = $r->call('package.search', $package, $summary, true, 
                $this->config->get('preferred_state') == 'stable', true);
        }
        if (PEAR::isError($available)) {
            $this->config->set('default_channel', $savechannel);
            return $this->raiseError($available);
        }
        if (!$available) {
            return $this->raiseError('no packages found that match pattern "' . $package . '"');
        }
        $data = array(
            'caption' => 'Matched packages, channel ' . $channel . ':',
            'border' => true,
            'headline' => array('Package', 'Stable/(Latest)', 'Local'),
            );

        foreach ($available as $name => $info) {
            $installed = $reg->packageInfo($name, null, $channel);
            $desc = $info['summary'];
            if (isset($params[$name]))
                $desc .= "\n\n".$info['description'];

            $unstable = '';
            if ($info['unstable']) {
                $unstable = '/(' . $info['unstable'] . ' ' . $info['state'] . ')';
            }
            if (!isset($info['stable']) || !$info['stable']) {
                $info['stable'] = 'none';
            }
            $version = is_array($installed['version']) ? $installed['version']['release'] :
                $installed['version'];
            $data['data'][$info['category']][] = array(
                $name,
                $info['stable'] . $unstable,
                $version,
                $desc,
                );
        }
        $this->ui->outputData($data, $command);
        $this->config->set('default_channel', $channel);
        return true;
    }

    // }}}
    function &getDownloader($options)
    {
        if (!class_exists('PEAR_Downloader')) {
            require_once 'PEAR/Downloader.php';
        }
        $a = &new PEAR_Downloader($this->ui, $options, $this->config);
        return $a;
    }
    // {{{ doDownload()

    function doDownload($command, $options, $params)
    {
        // make certain that dependencies are ignored
        $options['downloadonly'] = 1;
        $downloader = &$this->getDownloader($options);
        $downloader->setDownloadDir(getcwd());
        $errors = array();
        $downloaded = array();
        $err = $downloader->download($params);
        if (PEAR::isError($err)) {
            return $err;
        }
        $errors = $downloader->getErrorMsgs();
        if (count($errors)) {
            $errinfo = array();
            $errinfo['data'] = array($errors);
            $errinfo['headline'] = 'Download Errors';
            $this->ui->outputData($errinfo);
            return $this->raiseError("$command failed");
        }
        $downloaded = $downloader->getDownloadedPackages();
        foreach ($downloaded as $pkg) {
            $this->ui->outputData("File $pkg[file] downloaded", $command);
        }
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
        require_once 'PEAR/Common.php';
        $savechannel = $channel = $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        foreach ($reg->listChannels() as $channel) {
            $inst = array_flip($reg->listPackages($channel));
            if (!count($inst)) {
                continue;
            }
            if ($channel == '__uri') {
                continue;
            }
            $this->config->set('default_channel', $channel);
            if (empty($params[0])) {
                $state = $this->config->get('preferred_state');
            } else {
                $state = $params[0];
            }
            $caption = $channel . ' Available Upgrades';
            $chan = $reg->getChannel($channel);
            $this->_checkChannelForStatus($channel, $chan);
            if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
                  $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
                $rest = &$this->config->getREST('1.0', array());
                if (empty($state) || $state == 'any') {
                    $state = false;
                } else {
                    $caption .= ' (' . implode(', ', PEAR_Common::betterStates($state, true)) . ')';
                }
                $latest = $rest->listLatestUpgrades($base, $state, $inst, $channel, $reg);
            } else {
                $remote = &$this->config->getRemote();
                $remote->pushErrorHandling(PEAR_ERROR_RETURN);
                if (empty($state) || $state == 'any') {
                    $latest = $remote->call("package.listLatestReleases");
                } else {
                    $latest = $remote->call("package.listLatestReleases", $state);
                    $caption .= ' (' . implode(', ', PEAR_Common::betterStates($state, true)) . ')';
                }
                $remote->popErrorHandling();
            }
            if (PEAR::isError($latest)) {
                $this->ui->outputData($latest->getMessage());
                continue;
            }
            $caption .= ':';
            if (PEAR::isError($latest)) {
                $this->config->set('default_channel', $savechannel);
                return $latest;
            }
            $data = array(
                'caption' => $caption,
                'border' => 1,
                'headline' => array('Channel', 'Package', 'Local', 'Remote', 'Size'),
                );
            foreach ((array)$latest as $pkg => $info) {
                $package = strtolower($pkg);
                if (!isset($inst[$package])) {
                    // skip packages we don't have installed
                    continue;
                }
                extract($info);
                $inst_version = $reg->packageInfo($package, 'version', $channel);
                $inst_state   = $reg->packageInfo($package, 'release_state', $channel);
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
                $data['data'][] = array($channel, $pkg, "$inst_version ($inst_state)", "$version ($state)", $fs);
            }
            if (empty($data['data'])) {
                $this->ui->outputData('Channel ' . $channel . ': No upgrades available');
            } else {
                $this->ui->outputData($data, $command);
            }
        }
        $this->config->set('default_channel', $savechannel);
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
            if (preg_match('/^xmlrpc_cache_[a-z0-9]{32}$/', $ent) ||
                  preg_match('/rest.cache(file|id)$/', $ent)) {
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
