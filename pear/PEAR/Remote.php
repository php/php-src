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
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';
require_once 'PEAR/Config.php';

/**
 * This is a class for doing remote operations against the central
 * PEAR database.
 */
class PEAR_Remote extends PEAR
{
    // {{{ properties

    var $config = null;

    // }}}

    // {{{ PEAR_Remote(config_object)

    function PEAR_Remote(&$config)
    {
        $this->PEAR();
        $this->config = $config;
    }

    // }}}

    // {{{ call(method, [args...])

    function call($method)
    {
        if (!extension_loaded("xmlrpc")) {
            return $this->raiseError("xmlrpc support not loaded");
        }
        $method = str_replace("_", ".", $method);
        $request = xmlrpc_encode_request($method, $params);
        $server_host = $this->config->get("master_server");
        if (empty($server_host)) {
            return $this->raiseError("PEAR_Remote::call: no master_server configured");
        }
        $server_port = 80;
        $fp = @fsockopen($server_host, $server_port);
        if (!$fp) {
            return $this->raiseError("PEAR_Remote::call: fsockopen(`$server_host', $server_port) failed");
        }
        $len = strlen($request);
        $req_headers = "Host: $server_host:$server_port\r\n" .
             "Content-type: text/xml\r\n" .
             "Content-length: $len\r\n";
        $username = $this->config->get('username');
        $password = $this->config->get('password');
        if ($username && $password) {
            $tmp = base64_encode("$username:$password");
            $req_headers .= "Authorization: Basic $tmp\r\n";
        }
        fwrite($fp, ("POST /xmlrpc.php HTTP/1.0\r\n$req_headers\r\n$request"));
        $response = '';
        $line1 = fgets($fp, 2048);
        if (!preg_match('!^HTTP/[0-9\.]+ (\d+) (.*)!', $line1, &$matches)) {
            return $this->raiseError("PEAR_Remote: invalid HTTP response from XML-RPC server");
        }
        switch ($matches[1]) {
            case "200":
                break;
            case "401":
                if ($username && $password) {
                    return $this->raiseError("PEAR_Remote: authorization failed", 401);
                } else {
                    return $this->raiseError("PEAR_Remote: authorization required, please log in first", 401);
                }
            default:
                return $this->raiseError("PEAR_Remote: unexpected HTTP response: $matches[1] $matches[2]", (int)$matches[1]);
        }
        while (trim(fgets($fp, 2048)) != ''); // skip rest of headers
        while ($chunk = fread($fp, 10240)) {
            $response .= $chunk;
        }
        fclose($fp);
        $ret = xmlrpc_decode($response);
        if (is_array($ret) && isset($ret['__PEAR_TYPE__'])) {
            if ($ret['__PEAR_TYPE__'] == 'error') {
                if (isset($ret['__PEAR_CLASS__'])) {
                    $class = $ret['__PEAR_CLASS__'];
                } else {
                    $class = "PEAR_Error";
                }
                if ($ret['code']     === '') $ret['code']     = null;
                if ($ret['message']  === '') $ret['message']  = null;
                if ($ret['userinfo'] === '') $ret['userinfo'] = null;
                if (strtolower($class) == 'db_error') {
                    $ret = $this->raiseError(DB::errorMessage($ret['code']),
                                             $ret['code'], null, null,
                                             $ret['userinfo']);
                } else {
                    $ret = $this->raiseError($ret['message'], $ret['code'],
                                             null, null, $ret['userinfo']);
                }
            }
        }
        return $ret;
    }

    // }}}
}

?>