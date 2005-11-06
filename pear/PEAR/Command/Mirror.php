<?php
/**
 * PEAR_Command_Mirror (download-all command)
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
 * @author     Alexander Merz <alexmerz@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.2.0
 */

/**
 * base class
 */
require_once 'PEAR/Command/Common.php';

/**
 * PEAR commands for providing file mirrors
 *
 * @category   pear
 * @package    PEAR
 * @author     Alexander Merz <alexmerz@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.2.0
 */
class PEAR_Command_Mirror extends PEAR_Command_Common
{
    // {{{ properties

    var $commands = array(
        'download-all' => array(
            'summary' => 'Downloads each available package from the default channel',
            'function' => 'doDownloadAll',
            'shortcut' => 'da',
            'options' => array(
                'channel' =>
                    array(
                    'shortopt' => 'c',
                    'doc' => 'specify a channel other than the default channel',
                    'arg' => 'CHAN',
                    ),
                ),
            'doc' => '
Requests a list of available packages from the default channel ({config default_channel})
and downloads them to current working directory.  Note: only
packages within preferred_state ({config preferred_state}) will be downloaded'
            ),
        );

    // }}}

    // {{{ constructor

    /**
     * PEAR_Command_Mirror constructor.
     *
     * @access public
     * @param object PEAR_Frontend a reference to an frontend
     * @param object PEAR_Config a reference to the configuration data
     */
    function PEAR_Command_Mirror(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    /**
     * For unit-testing
     */
    function &factory($a)
    {
        $a = &PEAR_Command::factory($a, $this->config);
        return $a;
    }

    // {{{ doDownloadAll()
    /**
    * retrieves a list of avaible Packages from master server
    * and downloads them
    *
    * @access public
    * @param string $command the command
    * @param array $options the command options before the command
    * @param array $params the stuff after the command name
    * @return bool true if succesful
    * @throw PEAR_Error 
    */
    function doDownloadAll($command, $options, $params)
    {
        $savechannel = $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        $channel = isset($options['channel']) ? $options['channel'] :
            $this->config->get('default_channel');
        if (!$reg->channelExists($channel)) {
            $this->config->set('default_channel', $savechannel);
            return $this->raiseError('Channel "' . $channel . '" does not exist');
        }
        $this->config->set('default_channel', $channel);
        $this->ui->outputData('Using Channel ' . $this->config->get('default_channel'));
        $chan = $reg->getChannel($channel);
        if ($chan->supportsREST($this->config->get('preferred_mirror')) &&
              $base = $chan->getBaseURL('REST1.0', $this->config->get('preferred_mirror'))) {
            $rest = &$this->config->getREST('1.0', array());
            $remoteInfo = array_flip($rest->listPackages($base));
        } else {
            $remote = &$this->config->getRemote();
            $stable = ($this->config->get('preferred_state') == 'stable');
            $remoteInfo = $remote->call("package.listAll", true, $stable, false);
        }
        if (PEAR::isError($remoteInfo)) {
            return $remoteInfo;
        }
        $cmd = &$this->factory("download");
        if (PEAR::isError($cmd)) {
            return $cmd;
        }
        /**
         * Error handling not necessary, because already done by 
         * the download command
         */
        PEAR::staticPushErrorHandling(PEAR_ERROR_RETURN);
        $err = $cmd->run('download', array('downloadonly' => true), array_keys($remoteInfo));
        PEAR::staticPopErrorHandling();
        $this->config->set('default_channel', $savechannel);
        if (PEAR::isError($err)) {
            $this->ui->outputData($err->getMessage());
        }
        return true;
    }

    // }}}
}
