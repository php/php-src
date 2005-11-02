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
// | Authors: Stig Bakken <ssb@php.net>                                   |
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

/**
 * @nodep Gtk
 */
if ('@include_path@' != '@'.'include_path'.'@') {
    ini_set('include_path', '@include_path@');
    $raw = false;
} else {
    // this is a raw, uninstalled pear, either a cvs checkout, or php distro
    $raw = true;
}
define('PEAR_RUNTYPE', 'pecl');
require_once 'pearcmd.php';
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * mode: php
 * End:
 */
// vim600:syn=php

?>
