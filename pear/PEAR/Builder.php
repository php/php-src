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
// | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR/Common.php';

/**
 * Class to handle building (compiling) extensions.
 *
 * @author Stig Sæther Bakken <ssb@fast.no>
 */
class PEAR_Builder extends PEAR_Common
{
    // {{{ properties

    // }}}

    // {{{ constructor

    /**
     * PEAR_Builder constructor.
     *
     * @param object $ui user interface object (instance of PEAR_Frontend_*)
     *
     * @access public
     */
    function PEAR_Builder(&$ui)
    {
        $this->PEAR_Common();
        $this->setFrontendObject($ui);
    }

    function build($descfile, $callback = null)
    {
        if (PEAR::isError($info = $this->infoFromDescriptionFile($descfile))) {
            return $info;
        }
        $configure_command = "./configure";
        if (isset($info['configure_options'])) {
            foreach ($info['configure_options'] as $o) {
                $r = $this->ui->userDialog($o['prompt'], 'text', @$o['default']);
                if (substr($o['name'], 0, 5) == 'with-' &&
                    ($r == 'yes' || $r == 'autodetect')) {
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
            $err = $this->_runCommand($cmd, $callback);
            if (PEAR::isError($err)) {
                return $err;
            }
            if (!$err) {
                break;
            }
        }
        return true;

    }

    // }}}


    function _runCommand($command, $callback = null)
    {
        $pp = @popen($command, "r");
        if (!$pp) {
            return $this->raiseError("failed to run `$command'");
        }
        while ($line = fgets($pp, 1024)) {
            call_user_func($callback, 'output', $line);
        }
        pclose($pp);
        return true;
    }
}

?>
