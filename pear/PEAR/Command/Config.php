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

    // {{{ getCommands()

    /**
     * Return a list of all the commands defined by this class.
     * @return array list of commands
     * @access public
     */
    function getCommands()
    {
        return array('config-show' => 'Show All Settings',
                     'config-get' => 'Show One Setting',
                     'config-set' => 'Change Setting');
    }

    // }}}

    function getHelp($command)
    {
        switch ($command) {
            case 'config-show':
                $ret = array('[<layer>]', 'Displays the configuration');
                break;
            case 'config-get':
                $ret = array('<parameter> [<layer>]',
                             'Displays the value of the given parameter');
                break;
            case 'config-set':
                $ret = array('<parameter> <value> [<layer>]',
                             'Sets the value of a parameter in the config');
                break;
        }
        $ret[1] .= "\n".
                   "  <layer>    Where to store/get the configuration. The installer\n".
                   "             supports 'user' (per user conf) and 'system' (global conf)";
        return $ret;
    }
    // {{{ run()

    function run($command, $options, $params)
    {
        $cf = &$this->config;
        $failmsg = '';
        switch ($command) {
            case 'config-show': {
                // $params[0] -> the layer
                if ($error = $this->_checkLayer(@$params[0])) {
                    $failmsg .= $error;
                    break;
                }
                $keys = $cf->getKeys();
                sort($keys);
                $this->ui->startTable(array('caption' => 'Configuration:'));
                foreach ($keys as $key) {
                    $type = $cf->getType($key);
                    $value = $cf->get($key, @$params[0]);
                    if ($type == 'password' && $value) {
                        $value = '********';
                    }
                    if (empty($value)) {
                        $value = '<not set>';
                    }
                    $this->ui->tableRow(array($key, $value));
                }
                $this->ui->endTable();
                break;
            }
            case 'config-get': {
                // $params[0] -> the parameter
                // $params[1] -> the layer
                if ($error = $this->_checkLayer(@$params[1])) {
                    $failmsg .= $error;
                    break;
                }
                if (sizeof($params) < 1 || sizeof($params) > 2) {
                    $failmsg .= "config-get expects 1 or 2 parameters. Try \"help config-get\" for help";
                } elseif (sizeof($params) == 1) {
                    $this->ui->displayLine("$params[0] = " . $cf->get($params[0]));
                } else {
                    $this->ui->displayLine("($params[1])$params[0] = " .
                                           $cf->get($params[0], $params[1]));
                }
                break;
            }
            case 'config-set': {
                // $param[0] -> a parameter to set
                // $param[1] -> the value for the parameter
                // $param[2] -> the layer
                if (sizeof($params) < 2 || sizeof($params) > 3) {
                    $failmsg .= "config-set expects 2 or 3 parameters. Try \"help config-set\" for help";
                    break;
                }
                if ($error = $this->_checkLayer(@$params[2])) {
                    $failmsg .= $error;
                    break;
                }
                if (!call_user_func_array(array(&$cf, 'set'), $params))
                {
                    $failmsg = "config-set (" . implode(", ", $params) . ") failed";
                } else {
                    $cf->store();
                }
                break;
            }
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

    /**
    * Checks if a layer is defined or not
    *
    * @param string $layer The layer to search for
    * @return mixed False on no error or the error message
    */
    function _checkLayer($layer = null)
    {
        if (!empty($layer)) {
            $layers = $this->config->getLayers();
            if (!in_array($layer, $layers)) {
                return " only the layers: \"" . implode('" or "', $layers) . "\" are supported";
            }
        }
        return false;
    }

}

?>