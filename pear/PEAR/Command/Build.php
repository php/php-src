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

/**
 * PEAR commands for building extensions.
 *
 */
class PEAR_Command_Build extends PEAR_Command_Common
{
    var $commands = array(
        'build' => array(
            'summary' => 'Build an Extension From C Source',
            'function' => 'doBuild',
            'shortcut' => 'b',
            'options' => array(),
            'doc' => '[package.xml]
Builds one or more extensions contained in a package.'
            ),
        );

    /**
     * PEAR_Command_Build constructor.
     *
     * @access public
     */
    function PEAR_Command_Build(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    function doBuild($command, $options, $params)
    {
        if (sizeof($params) < 1) {
            $params[0] = 'package.xml';
        }
        $obj = &new PEAR_Common();
        if (PEAR::isError($info = $obj->infoFromAny($params[0]))) {
            return $info;
        }
        $configure_command = "./configure";
        if (isset($info['configure_options'])) {
            foreach ($info['configure_options'] as $o) {
                $r = $this->ui->userDialog($o['prompt'], 'text', @$o['default']);
                if ($r == 'yes' && substr($o['name'], 0, 5) == 'with-') {
                    $configure_command .= " --$o[name]";
                } else {
                    $configure_command .= " --$o[name]=$r";
                }
            }
        }
        if (isset($_ENV['MAKE'])) {
            $make_command = $_ENV['MAKE'];
        } else {
            $make_command = 'make';
        }
        $to_run = array(
            "phpize",
            $configure_command,
            $make_command,
            );
        foreach ($to_run as $cmd) {
            if (PEAR::isError($err = $this->_runCommand($cmd))) {
                return $err;
            }
        }
        return true;
    }

    function _runCommand($command)
    {
        $pp = @popen($command, "r");
        if (!$pp) {
            return $this->raiseError("failed to run `$command'");
        }
        while ($line = fgets($pp, 1024)) {
            $this->ui->displayLine(rtrim($line));
        }
        pclose($pp);
        
    }
}
