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
require_once "PEAR/Remote.php";
require_once "PEAR/Config.php";

/**
 * PEAR commands for managing configuration data.
 *
 */
class PEAR_Command_Auth extends PEAR_Command_Common
{
    // {{{ constructor

    /**
     * PEAR_Command_Auth constructor.
     *
     * @access public
     */
    function PEAR_Command_Auth(&$ui, &$config)
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
        return array('login', 'logout');
    }

    // }}}

    function getHelp($command)
    {
        switch ($command) {
            case 'login':
                return array(null, 'Connects to the remote server');
            case 'logout':
                return array(null, 'Disconnects from the remote server');
        }
    }
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
        $server = $this->config->get('master_server');
        switch ($command) {
            case 'login': {
                $remote = new PEAR_Remote($this->config);
                $username = $this->config->get('username');
                if (empty($username)) {
                    $username = @$_ENV['USER'];
                }
                $this->ui->displayLine("Logging in to $server.");
                $username = trim($this->ui->userDialog('Username', 'text', $username));

                $this->config->set('username', $username);
                $password = trim($this->ui->userDialog('Password', 'password'));
                $this->config->set('password', $password);
                $remote->expectError(401);
                $ok = $remote->call('logintest');
                $remote->popExpect();
                if ($ok === true) {
                    $this->ui->displayLine("Logged in.");
                    $this->config->store();
                } else {
                    $this->ui->displayLine("Login failed!");
                }
                break;
            }
            case 'logout': {
                $this->ui->displayLine("Logging out from $server.");
                $this->config->remove('username');
                $this->config->remove('password');
                $this->config->store();
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