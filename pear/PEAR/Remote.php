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
// $Id$

require_once 'PEAR.php';

/**
 * This is a class for doing remote operations against the central
 * PEAR database.
 */
class PEAR_Remote extends PEAR
{
    // {{{ properties

    var $config_object = null;

    // }}}

    // {{{ PEAR_Remote(config_object)

    function PEAR_Remote($config_object)
    {
        $this->PEAR();
        $this->config_object = $config_object;
    }

    // }}}

    // {{{ call(method, [args...])

    function call($method)
    {
        if (!extension_loaded("xmlrpc")) {
            return $this->raiseError("xmlrpc support not loaded");
        }
        $params = array_slice(func_get_args(), 1);
        $request = xmlrpc_encode_request($method, $params);
        print "<p><b>Request:</b><p>\n";
        print "<pre>";
        print htmlspecialchars($request);
        print "</pre>";
        $fp = @fsockopen($config->get("master_server"), 80);
        if (!$fp) {
            return $this->raiseError("PEAR_Remote::call: connect failed");
        }
        $len = strlen($request);
        fwrite($fp, ("POST /xmlrpc.php HTTP/1.0\r\n".
                     "Content-type: text/xml\r\n".
                     "Content-length: $len\r\n".
                     "\r\n$request"));
        print "Response:\n";
        while ($line = fgets($fp, 2048)) {
            print $line;
        }
        // XXX UNFINISHED
    }

    // }}}
}

?>