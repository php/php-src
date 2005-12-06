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
// | Author: Stig Bakken <ssb@fast.no>                                    |
// |         Tomas V.V.Cox <cox@idecnet.com>                              |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR/Command/Common.php";
require_once "PEAR/Config.php";

/**
 * PEAR commands for managing configuration data.
 *
 */
class PEAR_Command_Config extends PEAR_Command_Common
{
    // {{{ properties

    var $commands = array(
        'config-show' => array(
            'summary' => 'Show All Settings',
            'function' => 'doConfigShow',
            'shortcut' => 'csh',
            'options' => array(),
            'doc' => '
Displays all configuration values.  An optional argument
may be used to tell which configuration layer to display.  Valid
configuration layers are "user", "system" and "default".
',
            ),
        'config-get' => array(
            'summary' => 'Show One Setting',
            'function' => 'doConfigGet',
            'shortcut' => 'cg',
            'options' => array(),
            'doc' => '<parameter> [layer]
Displays the value of one configuration parameter.  The
first argument is the name of the parameter, an optional second argument
may be used to tell which configuration layer to look in.  Valid configuration
layers are "user", "system" and "default".  If no layer is specified, a value
will be picked from the first layer that defines the parameter, in the order
just specified.
',
            ),
        'config-set' => array(
            'summary' => 'Change Setting',
            'function' => 'doConfigSet',
            'shortcut' => 'cs',
            'options' => array(),
            'doc' => '<parameter> <value> [layer]
Sets the value of one configuration parameter.  The first argument is
the name of the parameter, the second argument is the new value.  Some
parameters are subject to validation, and the command will fail with
an error message if the new value does not make sense.  An optional
third argument may be used to specify in which layer to set the
configuration parameter.  The default layer is "user".
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
            return $this->raiseError($error);
        }
        $keys = $this->config->getKeys();
        sort($keys);
        $data = array('caption' => 'Configuration:');
        foreach ($keys as $key) {
            $type = $this->config->getType($key);
            $value = $this->config->get($key, @$params[0]);
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
            return $this->raiseError($error);
        }
        if (sizeof($params) < 1 || sizeof($params) > 2) {
            return $this->raiseError("config-get expects 1 or 2 parameters");
        } elseif (sizeof($params) == 1) {
            $this->ui->outputData("$params[0]=" . $this->config->get($params[0]), $command);
        } else {
            $data = "$params[1].$params[0]=" .$this->config->get($params[0], $params[1]);
            $this->ui->outputData($data, $command);
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
            return PEAR::raiseError($failmsg);
        }
        if (!call_user_func_array(array(&$this->config, 'set'), $params))
        {
            $failmsg = "config-set (" . implode(", ", $params) . ") failed";
        } else {
            $this->config->store();
        }
        if ($failmsg) {
            return $this->raiseError($failmsg);
        }
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