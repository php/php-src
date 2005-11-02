<?php
// /* vim: set expandtab tabstop=4 shiftwidth=4: */
/**
 * PEAR_Command_Channels (list-channels, update-channels, channel-delete, channel-add,
 * channel-update, channel-info, channel-alias, channel-discover commands)
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
 * @since      File available since Release 1.4.0a1
 */

/**
 * base class
 */
require_once 'PEAR/Command/Common.php';

/**
 * PEAR commands for managing channels.
 *
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_Command_Channels extends PEAR_Command_Common
{
    // {{{ properties

    var $commands = array(
        'list-channels' => array(
            'summary' => 'List Available Channels',
            'function' => 'doList',
            'shortcut' => 'lc',
            'options' => array(),
            'doc' => '
List all available channels for installation.
',
            ),
        'update-channels' => array(
            'summary' => 'Update the Channel List',
            'function' => 'doUpdateAll',
            'shortcut' => 'uc',
            'options' => array(),
            'doc' => '
List all installed packages in all channels.
'
            ),
        'channel-delete' => array(
            'summary' => 'Remove a Channel From the List',
            'function' => 'doDelete',
            'shortcut' => 'cde',
            'options' => array(),
            'doc' => '<channel name>
Delete a channel from the registry.  You may not
remove any channel that has installed packages.
'
            ),
        'channel-add' => array(
            'summary' => 'Add a Channel',
            'function' => 'doAdd',
            'shortcut' => 'ca',
            'options' => array(),
            'doc' => '<channel.xml>
Add a private channel to the channel list.  Note that all
public channels should be synced using "update-channels".
Parameter may be either a local file or remote URL to a
channel.xml.
'
            ),
        'channel-update' => array(
            'summary' => 'Update an Existing Channel',
            'function' => 'doUpdate',
            'shortcut' => 'cu',
            'options' => array(
                'force' => array(
                    'shortopt' => 'f',
                    'doc' => 'will force download of new channel.xml if an existing channel name is used',
                    ),
                'channel' => array(
                    'shortopt' => 'c',
                    'arg' => 'CHANNEL',
                    'doc' => 'will force download of new channel.xml if an existing channel name is used',
                    ),
),
            'doc' => '[<channel.xml>|<channel name>]
Update a channel in the channel list directly.  Note that all
public channels can be synced using "update-channels".
Parameter may be a local or remote channel.xml, or the name of
an existing channel.
'
            ),
        'channel-info' => array(
            'summary' => 'Retrieve Information on a Channel',
            'function' => 'doInfo',
            'shortcut' => 'ci',
            'options' => array(),
            'doc' => '<package>
List the files in an installed package.
'
            ),
        'channel-alias' => array(
            'summary' => 'Specify an alias to a channel name',
            'function' => 'doAlias',
            'shortcut' => 'cha',
            'options' => array(),
            'doc' => '<channel> <alias>
Specify a specific alias to use for a channel name.
The alias may not be an existing channel name or
alias.
'
            ),
        'channel-discover' => array(
            'summary' => 'Initialize a Channel from its server',
            'function' => 'doDiscover',
            'shortcut' => 'di',
            'options' => array(),
            'doc' => '<package>
List the files in an installed package.
'
            ),
        );

    // }}}
    // {{{ constructor

    /**
     * PEAR_Command_Registry constructor.
     *
     * @access public
     */
    function PEAR_Command_Channels(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ doList()
    
    function _sortChannels($a, $b)
    {
        return strnatcasecmp($a->getName(), $b->getName());
    }

    function doList($command, $options, $params)
    {
        $reg = &$this->config->getRegistry();
        $registered = $reg->getChannels();
        usort($registered, array(&$this, '_sortchannels'));
        $i = $j = 0;
        $data = array(
            'caption' => 'Registered Channels:',
            'border' => true,
            'headline' => array('Channel', 'Summary')
            );
        foreach ($registered as $channel) {
            $data['data'][] = array($channel->getName(),
                                      $channel->getSummary());
        }
        if (count($registered)==0) {
            $data = '(no registered channels)';
        }
        $this->ui->outputData($data, $command);
        return true;
    }
    
    function doUpdateAll($command, $options, $params)
    {
        $reg = &$this->config->getRegistry();
        $savechannel = $this->config->get('default_channel');
        if (isset($options['channel'])) {
            if (!$reg->channelExists($options['channel'])) {
                return $this->raiseError('Unknown channel "' . $options['channel'] . '"');
            }
            $this->config->set('default_channel', $options['channel']);
        } else {
            $this->config->set('default_channel', 'pear.php.net');
        }
        $remote = &$this->config->getRemote();
        $channels = $remote->call('channel.listAll');
        if (PEAR::isError($channels)) {
            $this->config->set('default_channel', $savechannel);
            return $channels;
        }
        if (!is_array($channels) || isset($channels['faultCode'])) {
            $this->config->set('default_channel', $savechannel);
            return $this->raiseError("Incorrect channel listing returned from channel '$chan'");
        }
        if (!count($channels)) {
            $data = 'no updates available';
        }
        $dl = &$this->getDownloader();
        if (!class_exists('System')) {
            require_once 'System.php';
        }
        $tmpdir = System::mktemp(array('-d'));
        foreach ($channels as $channel) {
            $channel = $channel[0];
            $save = $channel;
            if ($reg->channelExists($channel, true)) {
                $this->ui->outputData("Updating channel \"$channel\"", $command);
                $test = $reg->getChannel($channel, true);
                if (!$test) {
                    $this->ui->outputData("Channel '$channel' is corrupt in registry!", $command);
                    $lastmodified = false;
                } else {
                    $lastmodified = $test->lastModified();
                    
                }
                PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
                $contents = $dl->downloadHttp('http://' . $test->getName() . '/channel.xml',
                    $this->ui, $tmpdir, null, $lastmodified);
                PEAR::staticPopErrorHandling();
                if (PEAR::isError($contents)) {
                    $this->ui->outputData('ERROR: Cannot retrieve channel.xml for channel "' .
                        $test->getName() . '"', $command);
                    continue;
                }
                if (!$contents) {
                    $this->ui->outputData("Channel \"$channel\" is up-to-date", $command);
                    continue;
                }
                list($contents, $lastmodified) = $contents;
                $info = implode('', file($contents));
                if (!$info) {
                    $this->ui->outputData("Channel \"$channel\" is up-to-date", $command);
                    continue;
                }
                if (!class_exists('PEAR_ChannelFile')) {
                    require_once 'PEAR/ChannelFile.php';
                }
                $channelinfo = new PEAR_ChannelFile;
                $channelinfo->fromXmlString($info);
                if ($channelinfo->getErrors()) {
                    $this->ui->outputData("Downloaded channel data from channel \"$channel\" " . 
                        'is corrupt, skipping', $command);
                    continue;
                }
                $channel = $channelinfo;
                if ($channel->getName() != $save) {
                    $this->ui->outputData('ERROR: Security risk - downloaded channel ' .
                        'definition file for channel "'
                        . $channel->getName() . ' from channel "' . $save .
                        '".  To use anyway, use channel-update', $command);
                    continue;
                }
                $reg->updateChannel($channel, $lastmodified);
            } else {
                if ($reg->isAlias($channel)) {
                    $temp = &$reg->getChannel($channel);
                    $temp->setAlias($temp->getName(), true); // set the alias to the channel name
                    if ($reg->channelExists($temp->getName())) {
                        $this->ui->outputData('ERROR: existing channel "' . $temp->getName() .
                            '" is aliased to "' . $channel . '" already and cannot be ' .
                            're-aliased to "' . $temp->getName() . '" because a channel with ' .
                            'that name or alias already exists!  Please re-alias and try ' .
                            'again.', $command);
                        continue;
                    }
                }
                $this->ui->outputData("Adding new channel \"$channel\"", $command);
                PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
                $contents = $dl->downloadHttp('http://' . $channel . '/channel.xml',
                    $this->ui, $tmpdir, null, false);
                PEAR::staticPopErrorHandling();
                if (PEAR::isError($contents)) {
                    $this->ui->outputData('ERROR: Cannot retrieve channel.xml for channel "' .
                        $channel . '"', $command);
                    continue;
                }
                list($contents, $lastmodified) = $contents;
                $info = implode('', file($contents));
                if (!class_exists('PEAR_ChannelFile')) {
                    require_once 'PEAR/ChannelFile.php';
                }
                $channelinfo = new PEAR_Channelfile;
                $channelinfo->fromXmlString($info);
                if ($channelinfo->getErrors()) {
                    $this->ui->outputData("Downloaded channel data from channel \"$channel\"" .
                        ' is corrupt, skipping', $command);
                    continue;
                }
                $channel = $channelinfo;
                if ($channel->getName() != $save) {
                    $this->ui->outputData('ERROR: Security risk - downloaded channel ' .
                        'definition file for channel "'
                        . $channel->getName() . '" from channel "' . $save .
                        '".  To use anyway, use channel-update', $command);
                    continue;
                }
                $reg->addChannel($channel, $lastmodified);
            }
        }
        $this->config->set('default_channel', $savechannel);
        $this->ui->outputData('update-channels complete', $command);
        return true;
    }
    
    function doInfo($command, $options, $params)
    {
        if (sizeof($params) != 1) {
            return $this->raiseError("No channel specified");
        }
        $reg = &$this->config->getRegistry();
        $channel = strtolower($params[0]);
        if ($reg->channelExists($channel)) {
            $chan = $reg->getChannel($channel);
        } else {
            if (strpos($channel, '://')) {
                $downloader = &$this->getDownloader();
                if (!class_exists('System')) {
                    require_once 'System.php';
                }
                $tmpdir = System::mktemp(array('-d'));
                PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
                $loc = $downloader->downloadHttp($channel, $this->ui, $tmpdir);
                PEAR::staticPopErrorHandling();
                if (PEAR::isError($loc)) {
                    return $this->raiseError('Cannot open "' . $channel . '"');
                } else {
                    $contents = implode('', file($loc));
                }
            } else {
                $fp = @fopen($params[0], 'r');
                if (!$fp) {
                    if (@file_exists($params[0])) {
                        return $this->raiseError('Cannot open "' . $params[0] . '"');
                    } else {
                        return $this->raiseError('Unknown channel "' . $channel . '"');
                    }
                }
                $contents = '';
                while (!feof($fp)) {
                    $contents .= fread($fp, 1024);
                }
                fclose($fp);
            }
            if (!class_exists('PEAR_ChannelFile')) {
                require_once 'PEAR/ChannelFile.php';
            }
            $chan = new PEAR_ChannelFile;
            $chan->fromXmlString($contents);
            $chan->validate();
            if ($errs = $chan->getErrors(true)) {
                foreach ($errs as $err) {
                    $this->ui->outputData($err['level'] . ': ' . $err['message']);
                }
                return $this->raiseError('Channel file "' . $params[0] . '" is not valid');
            }
        }
        if ($chan) {
            $channel = $chan->getName();
            $caption = 'Channel ' . $channel . ' Information:';
            $data1 = array(
                'caption' => $caption,
                'border' => true);
            $data1['data']['server'] = array('Name and Server', $chan->getName());
            if ($chan->getAlias() != $chan->getName()) {
                $data1['data']['alias'] = array('Alias', $chan->getAlias());
            }
            $data1['data']['summary'] = array('Summary', $chan->getSummary());
            $validate = $chan->getValidationPackage();
            $data1['data']['vpackage'] = array('Validation Package Name', $validate['_content']);
            $data1['data']['vpackageversion'] =
                array('Validation Package Version', $validate['attribs']['version']);
            $d = array();
            $d['main'] = $data1;

            $data['data'] = array();
            $data['caption'] = 'Server Capabilities';
            $data['headline'] = array('Type', 'Version/REST type', 'Function Name/REST base');
            $capabilities = $chan->getFunctions('xmlrpc');
            $soaps = $chan->getFunctions('soap');
            if ($capabilities || $soaps || $chan->supportsREST()) {
                if ($capabilities) {
                    if (!isset($capabilities[0])) {
                        $capabilities = array($capabilities);
                    }
                    foreach ($capabilities as $protocol) {
                        $data['data'][] = array('xmlrpc', $protocol['attribs']['version'],
                            $protocol['_content']);
                    }
                }
                if ($soaps) {
                    if (!isset($soaps[0])) {
                        $soaps = array($soaps);
                    }
                    foreach ($soaps as $protocol) {
                        $data['data'][] = array('soap', $protocol['attribs']['version'],
                            $protocol['_content']);
                    }
                }
                if ($chan->supportsREST()) {
                    $funcs = $chan->getFunctions('rest');
                    if (!isset($funcs[0])) {
                        $funcs = array($funcs);
                    }
                    foreach ($funcs as $protocol) {
                        $data['data'][] = array('rest', $protocol['attribs']['type'],
                            $protocol['_content']); 
                    }
                }
            } else {
                $data['data'][] = array('No supported protocols');
            }
            $d['protocols'] = $data;
            $data['data'] = array();
            $mirrors = $chan->getMirrors();
            if ($mirrors) {
                $data['caption'] = 'Channel ' . $channel . ' Mirrors:';
                unset($data['headline']);
                foreach ($mirrors as $mirror) {
                    $data['data'][] = array($mirror['attribs']['host']);
                    $d['mirrors'] = $data;
                }
                foreach ($mirrors as $mirror) {
                    $data['data'] = array();
                    $data['caption'] = 'Mirror ' . $mirror['attribs']['host'] . ' Capabilities';
                    $data['headline'] = array('Type', 'Version/REST type', 'Function Name/REST base');
                    $capabilities = $chan->getFunctions('xmlrpc', $mirror['attribs']['host']);
                    $soaps = $chan->getFunctions('soap', $mirror['attribs']['host']);
                    if ($capabilities || $soaps || $chan->supportsREST($mirror['attribs']['host'])) {
                        if ($capabilities) {
                            if (!isset($capabilities[0])) {
                                $capabilities = array($capabilities);
                            }
                            foreach ($capabilities as $protocol) {
                                $data['data'][] = array('xmlrpc', $protocol['attribs']['version'],
                                    $protocol['_content']);
                            }
                        }
                        if ($soaps) {
                            if (!isset($soaps[0])) {
                                $soaps = array($soaps);
                            }
                            foreach ($soaps as $protocol) {
                                $data['data'][] = array('soap', $protocol['attribs']['version'],
                                    $protocol['_content']);
                            }
                        }
                        if ($chan->supportsREST($mirror['attribs']['host'])) {
                            $funcs = $chan->getFunctions('rest', $mirror['attribs']['host']);
                            if (!isset($funcs[0])) {
                                $funcs = array($funcs);
                            }
                            foreach ($funcs as $protocol) {
                                $data['data'][] = array('rest', $protocol['attribs']['type'],
                                    $protocol['_content']); 
                            }
                        }
                    } else {
                        $data['data'][] = array('No supported protocols');
                    }
                    $d['mirrorprotocols'] = $data;
                }
            }
            $this->ui->outputData($d, 'channel-info');
        } else {
            return $this->raiseError('Serious error: Channel "' . $params[0] .
                '" has a corrupted registry entry');
        }
    }

    // }}}
    
    function doDelete($command, $options, $params)
    {
        if (sizeof($params) != 1) {
            return $this->raiseError('channel-delete: no channel specified');
        }
        $reg = &$this->config->getRegistry();
        if (!$reg->channelExists($params[0])) {
            return $this->raiseError('channel-delete: channel "' . $params[0] . '" does not exist');
        }
        $channel = $reg->channelName($params[0]);
        if ($channel == 'pear.php.net') {
            return $this->raiseError('Cannot delete the pear.php.net channel');
        }
        if ($channel == 'pecl.php.net') {
            return $this->raiseError('Cannot delete the pecl.php.net channel');
        }
        if ($channel == '__uri') {
            return $this->raiseError('Cannot delete the __uri pseudo-channel');
        }
        if (PEAR::isError($err = $reg->listPackages($channel))) {
            return $err;
        }
        if (count($err)) {
            return $this->raiseError('Channel "' . $channel .
                '" has installed packages, cannot delete');
        }
        if (!$reg->deleteChannel($channel)) {
            return $this->raiseError('Channel "' . $channel . '" deletion failed');
        } else {
            $this->config->deleteChannel($channel);
            $this->ui->outputData('Channel "' . $channel . '" deleted', $command);
        }
    }

    function doAdd($command, $options, $params)
    {
        if (sizeof($params) != 1) {
            return $this->raiseError('channel-add: no channel file specified');
        }
        if (strpos($params[0], '://')) {
            $downloader = &$this->getDownloader();
            if (!class_exists('System')) {
                require_once 'System.php';
            }
            $tmpdir = System::mktemp(array('-d'));
            PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
            $loc = $downloader->downloadHttp($params[0], $this->ui, $tmpdir, null, false);
            PEAR::staticPopErrorHandling();
            if (PEAR::isError($loc)) {
                return $this->raiseError('channel-add: Cannot open "' . $params[0] . '"');
            } else {
                list($loc, $lastmodified) = $loc;
                $contents = implode('', file($loc));
            }
        } else {
            $lastmodified = false;
            $fp = @fopen($params[0], 'r');
            if (!$fp) {
                return $this->raiseError('channel-add: cannot open "' . $params[0] . '"');
            }
            $contents = '';
            while (!feof($fp)) {
                $contents .= fread($fp, 1024);
            }
            fclose($fp);
        }
        if (!class_exists('PEAR_ChannelFile')) {
            require_once 'PEAR/ChannelFile.php';
        }
        $channel = new PEAR_ChannelFile;
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $result = $channel->fromXmlString($contents);
        PEAR::staticPopErrorHandling();
        if (!$result) {
            $exit = false;
            if (count($errors = $channel->getErrors(true))) {
                foreach ($errors as $error) {
                    $this->ui->outputData(ucfirst($error['level'] . ': ' . $error['message']));
                    if (!$exit) {
                        $exit = $error['level'] == 'error' ? true : false;
                    }
                }
                if ($exit) {
                    return $this->raiseError('channel-add: invalid channel.xml file');
                }
            }
        }
        $reg = &$this->config->getRegistry();
        if ($reg->channelExists($channel->getName())) {
            return $this->raiseError('channel-add: Channel "' . $channel->getName() .
                '" exists, use channel-update to update entry');
        }
        $ret = $reg->addChannel($channel, $lastmodified);
        if (PEAR::isError($ret)) {
            return $ret;
        }
        if (!$ret) {
            return $this->raiseError('channel-add: adding Channel "' . $channel->getName() .
                '" to registry failed');
        }
        $this->config->setChannels($reg->listChannels());
        $this->config->writeConfigFile();
        $this->ui->outputData('Adding Channel "' . $channel->getName() . '" succeeded', $command);
    }

    function doUpdate($command, $options, $params)
    {
        if (!class_exists('System')) {
            require_once 'System.php';
        }
        $tmpdir = System::mktemp(array('-d'));
        $reg = &$this->config->getRegistry();
        if (sizeof($params) != 1) {
            return $this->raiseError("No channel file specified");
        }
        $lastmodified = false;
        if ((!file_exists($params[0]) || is_dir($params[0]))
              && $reg->channelExists(strtolower($params[0]))) {
            $c = $reg->getChannel(strtolower($params[0]));
            $this->ui->outputData('Retrieving channel.xml from remote server');
            $dl = &$this->getDownloader(array());
            // if force is specified, use a timestamp of "1" to force retrieval
            $lastmodified = isset($options['force']) ? false : $c->lastModified();
            PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
            $contents = $dl->downloadHttp('http://' . $c->getName() . '/channel.xml',
                $this->ui, $tmpdir, null, $lastmodified);
            PEAR::staticPopErrorHandling();
            if (PEAR::isError($contents)) {
                return $this->raiseError('Cannot retrieve channel.xml for channel "' .
                    $c->getName() . '"');
            }
            list($contents, $lastmodified) = $contents;
            if (!$contents) {
                $this->ui->outputData("Channel $params[0] channel.xml is up to date");
                return;
            }
            $contents = implode('', file($contents));
            if (!class_exists('PEAR_ChannelFile')) {
                require_once 'PEAR/ChannelFile.php';
            }
            $channel = new PEAR_ChannelFile;
            $channel->fromXmlString($contents);
            if (!$channel->getErrors()) {
                // security check: is the downloaded file for the channel we got it from?
                if (strtolower($channel->getName()) != strtolower($c->getName())) {
                    if (isset($options['force'])) {
                        $this->ui->log(0, 'WARNING: downloaded channel definition file' .
                            ' for channel "' . $channel->getName() . '" from channel "' .
                            strtolower($c->getName()) . '"');
                    } else {
                        return $this->raiseError('ERROR: downloaded channel definition file' .
                            ' for channel "' . $channel->getName() . '" from channel "' .
                            strtolower($c->getName()) . '"');
                    }
                }
            }
        } else {
            if (strpos($params[0], '://')) {
                $dl = &$this->getDownloader();
                PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
                $loc = $dl->downloadHttp($params[0],
                    $this->ui, $tmpdir, null, $lastmodified);
                PEAR::staticPopErrorHandling();
                if (PEAR::isError($loc)) {
                    return $this->raiseError("Cannot open " . $params[0]);
                } else {
                    list($loc, $lastmodified) = $loc;
                    $contents = implode('', file($loc));
                }
            } else {
                $fp = @fopen($params[0], 'r');
                if (!$fp) {
                    return $this->raiseError("Cannot open " . $params[0]);
                }
                $contents = '';
                while (!feof($fp)) {
                    $contents .= fread($fp, 1024);
                }
                fclose($fp);
            }
            if (!class_exists('PEAR_ChannelFile')) {
                require_once 'PEAR/ChannelFile.php';
            }
            $channel = new PEAR_ChannelFile;
            $channel->fromXmlString($contents);
        }
        $exit = false;
        if (count($errors = $channel->getErrors(true))) {
            foreach ($errors as $error) {
                $this->ui->outputData(ucfirst($error['level'] . ': ' . $error['message']));
                if (!$exit) {
                    $exit = $error['level'] == 'error' ? true : false;
                }
            }
            if ($exit) {
                return $this->raiseError('Invalid channel.xml file');
            }
        }
        if (!$reg->channelExists($channel->getName())) {
            return $this->raiseError('Error: Channel "' . $channel->getName() .
                '" does not exist, use channel-add to add an entry');
        }
        $ret = $reg->updateChannel($channel, $lastmodified);
        if (PEAR::isError($ret)) {
            return $ret;
        }
        if (!$ret) {
            return $this->raiseError('Updating Channel "' . $channel->getName() .
                '" in registry failed');
        }
        $this->config->setChannels($reg->listChannels());
        $this->config->writeConfigFile();
        $this->ui->outputData('Update of Channel "' . $channel->getName() . '" succeeded');
    }

    function &getDownloader()
    {
        if (!class_exists('PEAR_Downloader')) {
            require_once 'PEAR/Downloader.php';
        }
        $a = new PEAR_Downloader($this->ui, array(), $this->config);
        return $a;
    }

    function doAlias($command, $options, $params)
    {
        $reg = &$this->config->getRegistry();
        if (sizeof($params) == 1) {
            return $this->raiseError('No channel alias specified');
        }
        if (sizeof($params) != 2) {
            return $this->raiseError(
                'Invalid format, correct is: channel-alias channel alias');
        }
        if (!$reg->channelExists($params[0], true)) {
            if ($reg->isAlias($params[0])) {
                $extra = ' (use "channel-alias ' . $reg->channelName($params[0]) . ' ' .
                    strtolower($params[1]) . '")';
            } else {
                $extra = '';
            }
            return $this->raiseError('"' . $params[0] . '" is not a valid channel' . $extra);
        }
        if ($reg->isAlias($params[1])) {
            return $this->raiseError('Channel "' . $reg->channelName($params[1]) . '" is ' .
                'already aliased to "' . strtolower($params[1]) . '", cannot re-alias');
        }
        $chan = &$reg->getChannel($params[0]);
        if (!$chan) {
            return $this->raiseError('Corrupt registry?  Error retrieving channel "' . $params[0] .
                '" information');
        }
        // make it a local alias
        if (!$chan->setAlias(strtolower($params[1]), true)) {
            return $this->raiseError('Alias "' . strtolower($params[1]) .
                '" is not a valid channel alias');
        }
        $reg->updateChannel($chan);
        $this->ui->outputData('Channel "' . $chan->getName() . '" aliased successfully to "' .
            strtolower($params[1]) . '"');
    }

    function doDiscover($command, $options, $params)
    {
        $reg = &$this->config->getRegistry();
        if (sizeof($params) != 1) {
            return $this->raiseError("No channel server specified");
        }
        if ($reg->channelExists($params[0])) {
            if ($reg->isAlias($params[0])) {
                return $this->raiseError("A channel alias named \"$params[0]\" " .
                    'already exists, aliasing channel "' . $reg->channelName($params[0])
                    . '"');
            } else {
                return $this->raiseError("Channel \"$params[0]\" is already initialized");
            }
        }
        $this->pushErrorHandling(PEAR_ERROR_RETURN);
        $err = $this->doAdd($command, $options, array('http://' . $params[0] . '/channel.xml'));
        $this->popErrorHandling();
        if (PEAR::isError($err)) {
            return $this->raiseError("Discovery of channel \"$params[0]\" failed");
        }
        $this->ui->outputData("Discovery of channel \"$params[0]\" succeeded", $command);
    }
}
?>
