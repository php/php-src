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
// | Author: Stig Sæther Bakken <ssb@fast.no>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once "PEAR.php";

class PEAR_Command_Common extends PEAR
{
    // {{{ properties

    /**
     * PEAR_Config object used to pass user system and configuration
     * on when executing commands
     *
     * @var object
     */
    var $config;

    /**
     * User Interface object, for all interaction with the user.
     * @var object
     */
    var $ui;

    // }}}
    // {{{ constructor

    /**
     * PEAR_Command_Common constructor.
     *
     * @access public
     */
    function PEAR_Command_Common(&$ui, &$config)
    {
        parent::PEAR();
        $this->config = &$config;
        $this->ui = &$ui;
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
        $ret = array();
        foreach (array_keys($this->commands) as $command) {
            $ret[$command] = $this->commands[$command]['summary'];
        }
        return $ret;
    }

    // }}}
    // {{{ getOptions()

    function getOptions($command)
    {
        return @$this->commands[$command]['options'];
    }

    // }}}
    // {{{ getGetoptArgs()

    function getGetoptArgs($command, &$short_args, &$long_args)
    {
        $short_args = "";
        $long_args = array();
        if (empty($this->commands[$command])) {
            return;
        }
        reset($this->commands[$command]);
        while (list($option, $info) = each($this->commands[$command]['options'])) {
            $larg = $sarg = '';
            if (isset($info['arg'])) {
                if ($info['arg']{0} == '(') {
                    $larg = '==';
                    $sarg = '::';
                    $arg = substr($info['arg'], 1, -1);
                } else {
                    $larg = '=';
                    $sarg = ':';
                    $arg = $info['arg'];
                }
            }
            if (isset($info['shortopt'])) {
                $short_args .= $info['shortopt'] . $sarg;
            }
            $long_args[] = $option . $larg;
        }
    }

    // }}}
    // {{{ getHelp()

    function getHelp($command)
    {
        $help = preg_replace('/{config\s+([^\}]+)}/e', "\$config->get('\1')", @$this->commands[$command]['doc']);
        return $help;
    }

    // }}}
    // {{{ run()

    function run($command, $options, $params)
    {
        $func = @$this->commands[$command]['function'];
        if (empty($func)) {
            return $this->raiseError("unknown command `$command'");
        }
        return $this->$func($command, $options, $params);
    }

    // }}}
}

?>