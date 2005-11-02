<?php
/**
 * PEAR_Command_Config (config-show, config-get, config-set, config-help, config-create commands)
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

/**
 * PEAR commands for managing configuration data.
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
class PEAR_Command_Config extends PEAR_Command_Common
{
    // {{{ properties

    var $commands = array(
        'config-show' => array(
            'summary' => 'Show All Settings',
            'function' => 'doConfigShow',
            'shortcut' => 'csh',
            'options' => array(
                'channel' => array(
                    'shortopt' => 'c',
                    'doc' => 'show configuration variables for another channel',
                    'arg' => 'CHAN',
                    ),
),
            'doc' => '[layer]
Displays all configuration values.  An optional argument
may be used to tell which configuration layer to display.  Valid
configuration layers are "user", "system" and "default". To display
configurations for different channels, set the default_channel
configuration variable and run config-show again.
',
            ),
        'config-get' => array(
            'summary' => 'Show One Setting',
            'function' => 'doConfigGet',
            'shortcut' => 'cg',
            'options' => array(
                'channel' => array(
                    'shortopt' => 'c',
                    'doc' => 'show configuration variables for another channel',
                    'arg' => 'CHAN',
                    ),
),
            'doc' => '<parameter> [layer]
Displays the value of one configuration parameter.  The
first argument is the name of the parameter, an optional second argument
may be used to tell which configuration layer to look in.  Valid configuration
layers are "user", "system" and "default".  If no layer is specified, a value
will be picked from the first layer that defines the parameter, in the order
just specified.  The configuration value will be retrieved for the channel
specified by the default_channel configuration variable.
',
            ),
        'config-set' => array(
            'summary' => 'Change Setting',
            'function' => 'doConfigSet',
            'shortcut' => 'cs',
            'options' => array(
                'channel' => array(
                    'shortopt' => 'c',
                    'doc' => 'show configuration variables for another channel',
                    'arg' => 'CHAN',
                    ),
),
            'doc' => '<parameter> <value> [layer]
Sets the value of one configuration parameter.  The first argument is
the name of the parameter, the second argument is the new value.  Some
parameters are subject to validation, and the command will fail with
an error message if the new value does not make sense.  An optional
third argument may be used to specify in which layer to set the
configuration parameter.  The default layer is "user".  The
configuration value will be set for the current channel, which
is controlled by the default_channel configuration variable.
',
            ),
        'config-help' => array(
            'summary' => 'Show Information About Setting',
            'function' => 'doConfigHelp',
            'shortcut' => 'ch',
            'options' => array(),
            'doc' => '[parameter]
Displays help for a configuration parameter.  Without arguments it
displays help for all configuration parameters.
',
           ),
        'config-create' => array(
            'summary' => 'Create a Default configuration file',
            'function' => 'doConfigCreate',
            'shortcut' => 'coc',
            'options' => array(
                'windows' => array(
                    'shortopt' => 'w',
                    'doc' => 'create a config file for a windows install',
                    ),
            ),
            'doc' => '<root path> <filename>
Create a default configuration file with all directory configuration
variables set to subdirectories of <root path>, and save it as <filename>.
This is useful especially for creating a configuration file for a remote
PEAR installation (using the --remoteconfig option of install, upgrade,
and uninstall).
',
            ),
        );

    // }}}
    // {{{ constructor

    /**
     * PEAR_Command_Config constructor.
     *
     * @access public
     */
    function PEAR_Command_Config(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ doConfigShow()

    function doConfigShow($command, $options, $params)
    {
        // $params[0] -> the layer
        if ($error = $this->_checkLayer(@$params[0])) {
            return $this->raiseError("config-show:$error");
        }
        $keys = $this->config->getKeys();
        sort($keys);
        $channel = isset($options['channel']) ? $options['channel'] :
            $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        if (!$reg->channelExists($channel)) {
            return $this->raiseError('Channel "' . $channel . '" does not exist');
        }
        $data = array('caption' => 'Configuration (channel ' . $channel . '):');
        foreach ($keys as $key) {
            $type = $this->config->getType($key);
            $value = $this->config->get($key, @$params[0], $channel);
            if ($type == 'password' && $value) {
                $value = '********';
            }
            if ($value === false) {
                $value = 'false';
            } elseif ($value === true) {
                $value = 'true';
            }
            $data['data'][$this->config->getGroup($key)][] = array($this->config->getPrompt($key) , $key, $value);
        }
        foreach ($this->config->getLayers() as $layer) {
            $data['data']['Config Files'][] = array(ucfirst($layer) . ' Configuration File', 'Filename' , $this->config->getConfFile($layer));
        }
        
        $this->ui->outputData($data, $command);
        return true;
    }

    // }}}
    // {{{ doConfigGet()

    function doConfigGet($command, $options, $params)
    {
        // $params[0] -> the parameter
        // $params[1] -> the layer
        if ($error = $this->_checkLayer(@$params[1])) {
            return $this->raiseError("config-get:$error");
        }
        $channel = isset($options['channel']) ? $options['channel'] :
            $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        if (!$reg->channelExists($channel)) {
            return $this->raiseError('Channel "' . $channel . '" does not exist');
        }
        if (sizeof($params) < 1 || sizeof($params) > 2) {
            return $this->raiseError("config-get expects 1 or 2 parameters");
        } else {
            if (count($params) == 1) {
                $layer = null;
            } else {
                $layer = $params[1];
            }
            $this->ui->outputData($this->config->get($params[0], $layer, $channel), $command);
        }
        return true;
    }

    // }}}
    // {{{ doConfigSet()

    function doConfigSet($command, $options, $params)
    {
        // $param[0] -> a parameter to set
        // $param[1] -> the value for the parameter
        // $param[2] -> the layer
        $failmsg = '';
        if (sizeof($params) < 2 || sizeof($params) > 3) {
            $failmsg .= "config-set expects 2 or 3 parameters";
            return PEAR::raiseError($failmsg);
        }
        if ($error = $this->_checkLayer(@$params[2])) {
            $failmsg .= $error;
            return PEAR::raiseError("config-set:$failmsg");
        }
        $channel = isset($options['channel']) ? $options['channel'] :
            $this->config->get('default_channel');
        $reg = &$this->config->getRegistry();
        if (!$reg->channelExists($channel)) {
            return $this->raiseError('Channel "' . $channel . '" does not exist');
        }
        if ($params[0] == 'default_channel') {
            if (!$reg->channelExists($params[1])) {
                return $this->raiseError('Channel "' . $params[1] . '" does not exist');
            }
        }
        if (count($params) == 2) {
            array_push($params, 'user');
            $layer = 'user';
        } else {
            $layer = $params[2];
        }
        array_push($params, $channel);
        if (!call_user_func_array(array(&$this->config, 'set'), $params))
        {
            array_pop($params);
            $failmsg = "config-set (" . implode(", ", $params) . ") failed, channel $channel";
        } else {
            $this->config->store($layer);
        }
        if ($failmsg) {
            return $this->raiseError($failmsg);
        }
        $this->ui->outputData('config-set succeeded', $command);
        return true;
    }

    // }}}
    // {{{ doConfigHelp()

    function doConfigHelp($command, $options, $params)
    {
        if (empty($params)) {
            $params = $this->config->getKeys();
        }
        $data['caption']  = "Config help" . ((count($params) == 1) ? " for $params[0]" : '');
        $data['headline'] = array('Name', 'Type', 'Description');
        $data['border']   = true;
        foreach ($params as $name) {
            $type = $this->config->getType($name);
            $docs = $this->config->getDocs($name);
            if ($type == 'set') {
                $docs = rtrim($docs) . "\nValid set: " .
                    implode(' ', $this->config->getSetValues($name));
            }
            $data['data'][] = array($name, $type, $docs);
        }
        $this->ui->outputData($data, $command);
    }

    // }}}
    // {{{ doConfigCreate()

    function doConfigCreate($command, $options, $params)
    {
        if (count($params) != 2) {
            return PEAR::raiseError('config-create: must have 2 parameters, root path and ' .
                'filename to save as');
        }
        $root = $params[0];
        // Clean up the DIRECTORY_SEPARATOR mess
        $ds2 = DIRECTORY_SEPARATOR . DIRECTORY_SEPARATOR;
        $root = preg_replace(array('!\\\\+!', '!/+!', "!$ds2+!"),
                             array('/', '/', '/'),
                            $root);
        if ($root{0} != '/') {
            if (isset($options['windows'])) {
                if (!preg_match('/^[A-Za-z]:/', $root)) {
                    return PEAR::raiseError('Root directory must be an absolute path beginning ' .
                        'with "\\" or "C:\\", was: "' . $root . '"');
                }
            } else {
                return PEAR::raiseError('Root directory must be an absolute path beginning ' .
                    'with "/", was: "' . $root . '"');
            }
        }
        $windows = isset($options['windows']);
        if ($windows) {
            $root = str_replace('/', '\\', $root);
        }
        if (!file_exists($params[1])) {
            if (!@touch($params[1])) {
                return PEAR::raiseError('Could not create "' . $params[1] . '"');
            }
        }
        $params[1] = realpath($params[1]);
        $config = &new PEAR_Config($params[1], '#no#system#config#', false, false);
        if ($root{strlen($root) - 1} == '/') {
            $root = substr($root, 0, strlen($root) - 1);
        }
        $config->noRegistry();
        $config->set('php_dir', $windows ? "$root\\pear\\php" : "$root/pear/php", 'user');
        $config->set('data_dir', $windows ? "$root\\pear\\data" : "$root/pear/data");
        $config->set('ext_dir', $windows ? "$root\\pear\\ext" : "$root/pear/ext");
        $config->set('doc_dir', $windows ? "$root\\pear\\docs" : "$root/pear/docs");
        $config->set('test_dir', $windows ? "$root\\pear\\tests" : "$root/pear/tests");
        $config->set('cache_dir', $windows ? "$root\\pear\\cache" : "$root/pear/cache");
        $config->set('bin_dir', $windows ? "$root\\pear" : "$root/pear");
        $config->writeConfigFile();
        $this->_showConfig($config);
        $this->ui->outputData('Successfully created default configuration file "' . $params[1] . '"',
            $command);
    }

    // }}}

    function _showConfig(&$config)
    {
        $params = array('user');
        $keys = $config->getKeys();
        sort($keys);
        $channel = 'pear.php.net';
        $data = array('caption' => 'Configuration (channel ' . $channel . '):');
        foreach ($keys as $key) {
            $type = $config->getType($key);
            $value = $config->get($key, 'user', $channel);
            if ($type == 'password' && $value) {
                $value = '********';
            }
            if ($value === false) {
                $value = 'false';
            } elseif ($value === true) {
                $value = 'true';
            }
            $data['data'][$config->getGroup($key)][] =
                array($config->getPrompt($key) , $key, $value);
        }
        foreach ($config->getLayers() as $layer) {
            $data['data']['Config Files'][] =
                array(ucfirst($layer) . ' Configuration File', 'Filename' ,
                    $config->getConfFile($layer));
        }
        
        $this->ui->outputData($data, 'config-show');
        return true;
    }
    // {{{ _checkLayer()

    /**
     * Checks if a layer is defined or not
     *
     * @param string $layer The layer to search for
     * @return mixed False on no error or the error message
     */
    function _checkLayer($layer = null)
    {
        if (!empty($layer) && $layer != 'default') {
            $layers = $this->config->getLayers();
            if (!in_array($layer, $layers)) {
                return " only the layers: \"" . implode('" or "', $layers) . "\" are supported";
            }
        }
        return false;
    }

    // }}}
}

?>
