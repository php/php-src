<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: Stig Bakken <ssb@php.net>                                    |
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
        $this->debug = $this->config->get('verbose');
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
        if (($what == 'cmdoutput' && $this->debug > 1) ||
            ($what == 'output' && $this->debug > 0)) {
            $this->ui->outputData(rtrim($data), 'build');
        }
    }

    // }}}
}
