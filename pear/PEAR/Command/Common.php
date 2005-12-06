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

    var $_deps_rel_trans = array(
                                 'lt' => '<',
                                 'le' => '<=',
                                 'eq' => '=',
                                 'ne' => '!=',
                                 'gt' => '>',
                                 'ge' => '>=',
                                 'has' => '=='
                                 );

    var $_deps_type_trans = array(
                                  'pkg' => 'package',
                                  'extension' => 'extension',
                                  'php' => 'PHP',
                                  'prog' => 'external program',
                                  'ldlib' => 'external library for linking',
                                  'rtlib' => 'external runtime library',
                                  'os' => 'operating system',
                                  'websrv' => 'web server',
                                  'sapi' => 'SAPI backend'
                                  );

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
    // {{{ getShortcuts()

    /**
     * Return a list of all the command shortcuts defined by this class.
     * @return array shortcut => command
     * @access public
     */
    function getShortcuts()
    {
        $ret = array();
        foreach (array_keys($this->commands) as $command) {
            if (isset($this->commands[$command]['shortcut'])) {
                $ret[$this->commands[$command]['shortcut']] = $command;
            }
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
    /**
    * Returns the help message for the given command
    *
    * @param string $command The command
    * @return mixed A fail string if the command does not have help or
    *               a two elements array containing [0]=>help string,
    *               [1]=> help string for the accepted cmd args
    */
    function getHelp($command)
    {
        $config = &PEAR_Config::singleton();
        $help = @$this->commands[$command]['doc'];
        if (empty($help)) {
            // XXX (cox) Fallback to summary if there is no doc (show both?)
            if (!$help = @$this->commands[$command]['summary']) {
                return "No help for command \"$command\"";
            }
        }
        if (preg_match_all('/{config\s+([^\}]+)}/e', $help, $matches)) {
            foreach($matches[0] as $k => $v) {
                $help = preg_replace("/$v/", $config->get($matches[1][$k]), $help);
            }
        }
        return array($help, $this->getHelpArgs($command));
    }

    // }}}
    // {{{ getHelpArgs()
    /**
    * Returns the help for the accepted arguments of a command
    *
    * @param  string $command
    * @return string The help string
    */
    function getHelpArgs($command)
    {
        if (isset($this->commands[$command]['options']) &&
            count($this->commands[$command]['options']))
        {
            $help = "Options:\n";
            foreach ($this->commands[$command]['options'] as $k => $v) {
                if (isset($v['arg'])) {
                    if ($v['arg']{0} == '(') {
                        $arg = substr($v['arg'], 1, -1);
                        $sapp = " [$arg]";
                        $lapp = "[=$arg]";
                    } else {
                        $sapp = " $v[arg]";
                        $lapp = "=$v[arg]";
                    }
                } else {
                    $sapp = $lapp = "";
                }
                if (isset($v['shortopt'])) {
                    $s = $v['shortopt'];
                    @$help .= "  -$s$sapp, --$k$lapp\n";
                } else {
                    @$help .= "  --$k$lapp\n";
                }
                $p = "        ";
                $doc = rtrim(str_replace("\n", "\n$p", $v['doc']));
                $help .= "        $doc\n";
            }
            return $help;
        }
        return null;
    }

    // }}}
    // {{{ run()

    function run($command, $options, $params)
    {
        $func = @$this->commands[$command]['function'];
        if (empty($func)) {
            // look for shortcuts
            foreach (array_keys($this->commands) as $cmd) {
                if (@$this->commands[$cmd]['shortcut'] == $command) {
                    $command = $cmd;
                    $func = @$this->commands[$command]['function'];
                    if (empty($func)) {
                        return $this->raiseError("unknown command `$command'");
                    }
                    break;
                }
            }
        }
        return $this->$func($command, $options, $params);
    }

    // }}}
}

?>