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
        return array('config-show', 'config-get', 'config-set');
    }

    // }}}
    // {{{ run()

    function run($command, $options, $params)
    {
        $cf =& $this->config;
        $failmsg = '';
        switch ($command) {
            case 'config-show': {
                $keys = $cf->getKeys();
                sort($keys);
                $this->ui->startTable(array('caption' => 'Configuration:'));
                if (isset($params[0]) && $cf->isDefined($params[0])) {
                    foreach ($keys as $key) {
                        $type = $cf->getType($key);
                        $value = $cf->get($key, $params[0]);
                        if ($type == 'password' && $value) {
                            $value = '********';
                        }
                        $this->ui->tableRow(array($key, $value));
                    }
                } else {
                    foreach ($keys as $key) {
                        $type = $cf->getType($key);
                        $value = $cf->get($key, $params[0]);
                        if ($type == 'password' && $value) {
                            $value = '********';
                        }
                        $this->ui->tableRow(array($key, $value));
                    }
                }
                $this->ui->endTable();
                break;
            }
            case 'config-get': {
                if (sizeof($params) < 1 || sizeof($params) > 2) {
                    $failmsg .= "config-get expects 1 or 2 parameters";
                } elseif (sizeof($params) == 1) {
                    $this->ui->displayLine("$params[0] = " . $cf->get($params[0]));
                } else {
                    $this->ui->displayLine("($params[1])$params[0] = " .
                                           $cf->get($params[0], $params[1]));
                }
                break;
            }
            case 'config-set': {
                if (sizeof($params) < 2 || sizeof($params) > 3) {
                    $failmsg .= "config-set expects 2 or 3 parameters";
                    break;
                } else {
                    if (!call_user_func_array(array($cf, 'set'), $params))
                    {
                        $failmsg = "config-set (" .
                             implode(", ", $params) . ") failed";
                    }
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
}

?>