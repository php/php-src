<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
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
require_once "PEAR/Builder.php";

/**
 * PEAR commands for building extensions.
 *
 */
class PEAR_Command_Build extends PEAR_Command_Common
{
    // {{{ properties

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

    // }}}

    // {{{ constructor

    /**
     * PEAR_Command_Build constructor.
     *
     * @access public
     */
    function PEAR_Command_Build(&$ui, &$config)
    {
        parent::PEAR_Command_Common($ui, $config);
    }

    // }}}

    // {{{ doBuild()

    function doBuild($command, $options, $params)
    {
        if (sizeof($params) < 1) {
            $params[0] = 'package.xml';
        }
        $builder = &new PEAR_Builder($this->ui);
        $this->verbose = $this->config->get('verbose');
        $err = $builder->build($params[0], array(&$this, 'buildCallback'));
        if (PEAR::isError($err)) {
            return $err;
        }
        return true;
    }

    // }}}
    // {{{ buildCallback()

    function buildCallback($what, $data)
    {
        if (($what == 'cmdoutput' && $this->verbose > 1) ||
            ($what == 'output' && $this->verbose > 0)) {
            $this->ui->outputData(rtrim($data), 'build');
        }
    }

    // }}}
}
