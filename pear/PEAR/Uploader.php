<?php
//
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2001 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//

require_once "PEAR/Common.php";

/**
 * Administration class used to install PEAR packages and maintain the
 * installed package database.
 *
 * @since PHP 4.0.2
 * @author Stig Bakken <ssb@fast.no>
 */
class PEAR_Uploader extends PEAR_Common
{
    // {{{ properties

    var $_tempfiles = array();

    // }}}

    // {{{ constructor

    function PEAR_Uploader()
    {
        $this->PEAR_Common();
    }

    // }}}

    function upload($pkgfile, $infofile = null)
    {
        if ($infofile === null) {
            $info = $this->infoFromTarBall($pkgfile);
        } else {
            $info = $this->infoFromDescriptionFile($infofile);
        }
        if (PEAR::isError($info)) {
            return $info;
        }
        
    }
}

?>
