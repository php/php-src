<?php
/**
 * PEAR_Command_Auth (build command)
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.0 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V.V.Cox <cox@idecnet.com>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * base class
 */
require_once 'PEAR/Command/Common.php';

/**
 * PEAR commands for building extensions.
 *
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Tomas V.V.Cox <cox@idecnet.com>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
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
        require_once 'PEAR/Builder.php';
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
