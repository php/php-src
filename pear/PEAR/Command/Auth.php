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
    // {{{ properties

    var $commands = array(
        'login' => array(
            'summary' => 'Connects and authenticates to remote server',
            'shortcut' => 'li',
            'function' => 'doLogin',
            'options' => array(),
            'doc' => '
Log in to the remote server.  To use remote functions in the installer
that require any kind of privileges, you need to log in first.  The
username and password you enter here will be stored in your per-user
PEAR configuration (~/.pearrc on Unix-like systems).  After logging
in, your username and password will be sent along in subsequent
operations on the remote server.',
            ),
        'logout' => array(
            'summary' => 'Logs out from the remote server',
            'shortcut' => 'lo',
            'function' => 'doLogout',
            'options' => array(),
            'doc' => '
Logs out from the remote server.  This command does not actually
connect to the remote server, it only deletes the stored username and
password from your user configuration.',
            )

        );

    // }}}

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

    // {{{ doLogin()

    /**
     * Execute the 'login' command.
     *
     * @param string $command command name
     *
     * @param array $options option_name => value
     *
     * @param array $params list of additional parameters
     *
     * @return bool TRUE on success, FALSE for unknown commands, or
     * a PEAR error on failure
     *
     * @access public
     */
    function doLogin($command, $options, $params)
    {
        $server = $this->config->get('master_server');
        $remote = new PEAR_Remote($this->config);
        $username = $this->config->get('username');
        if (empty($username)) {
            $username = @$_ENV['USER'];
        }
        $this->ui->outputData("Logging in to $server.", $command);
        
        list($username, $password) = $this->ui->userDialog(
            $command,
            array('Username', 'Password'),
            array('text',     'password'),
            array($username,  '')
            );
        $username = trim($username);
        $password = trim($password);
        
        $this->config->set('username', $username);
        $this->config->set('password', $password);
        
        $remote->expectError(401);
        $ok = $remote->call('logintest');
        $remote->popExpect();
        if ($ok === true) {
            $this->ui->outputData("Logged in.", $command);
            $this->config->store();
        } else {
            return $this->raiseError("Login failed!");
        }

    }

    // }}}
    // {{{ doLogout()

    /**
     * Execute the 'logout' command.
     *
     * @param string $command command name
     *
     * @param array $options option_name => value
     *
     * @param array $params list of additional parameters
     *
     * @return bool TRUE on success, FALSE for unknown commands, or
     * a PEAR error on failure
     *
     * @access public
     */
    function doLogout($command, $options, $params)
    {
        $server = $this->config->get('master_server');
        $this->ui->outputData("Logging out from $server.", $command);
        $this->config->remove('username');
        $this->config->remove('password');
        $this->config->store();
    }

    // }}}
}

?>