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
// | Author: Stig Bakken <ssb@php.net>                                    |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';
require_once 'PEAR/Config.php';

/**
 * This is a class for doing remote operations against the central
 * PEAR database.
 *
 * @nodep XML_RPC_Value
 * @nodep XML_RPC_Message
 * @nodep XML_RPC_Client
 */
class PEAR_Remote extends PEAR
{
    // {{{ properties

    var $config = null;
    var $cache  = null;

    // }}}

    // {{{ PEAR_Remote(config_object)

    function PEAR_Remote(&$config)
    {
        $this->PEAR();
        $this->config = &$config;
    }

    // }}}

    // {{{ getCache()

    
    function getCache($args)
    {
        $id       = md5(serialize($args));
        $cachedir = $this->config->get('cache_dir');
        if (!file_exists($cachedir)) {
            System::mkdir('-p '.$cachedir);
        }
        $filename = $cachedir . DIRECTORY_SEPARATOR . 'xmlrpc_cache_' . $id;
        if (!file_exists($filename)) {
            return null;
        };
		
        $fp = fopen($filename, "rb");
        if ($fp === null) {
            return null;
        }
        $content  = fread($fp, filesize($filename));
        fclose($fp);
        $result   = array(
            'age'        => time() - filemtime($filename),
            'lastChange' => filemtime($filename),
            'content'    => unserialize($content),
            );
        return $result;
    }

    // }}}
    
    // {{{ saveCache()

    function saveCache($args, $data)
    {
        $id       = md5(serialize($args));
        $cachedir = $this->config->get('cache_dir');
        if (!file_exists($cachedir)) {
            System::mkdir('-p '.$cachedir);
        }
        $filename = $cachedir.'/xmlrpc_cache_'.$id;
        
        $fp = @fopen($filename, "wb");
        if ($fp !== null) {
            fwrite($fp, serialize($data));
            fclose($fp);
        };
    }

    // }}}
    
    // {{{ call(method, [args...])

    function call($method)
    {
        $_args = $args = func_get_args();
        
        $this->cache = $this->getCache($args);
        $cachettl = $this->config->get('cache_ttl');
        // If cache is newer than $cachettl seconds, we use the cache!
        if ($this->cache !== null && $this->cache['age'] < $cachettl) {
            return $this->cache['content'];
        };
        
        if (extension_loaded("xmlrpc")) {
            $result = call_user_func_array(array(&$this, 'call_epi'), $args);
            if (!PEAR::isError($result)) {
                $this->saveCache($_args, $result);
            };
            return $result;
        }
        if (!@include_once("XML/RPC.php")) {
            return $this->raiseError("For this remote PEAR operation you need to install the XML_RPC package");
        }
        array_shift($args);
        $server_host = $this->config->get('master_server');
        $username = $this->config->get('username');
        $password = $this->config->get('password');
        $eargs = array();
        foreach($args as $arg) $eargs[] = $this->_encode($arg);
        $f = new XML_RPC_Message($method, $eargs);
        if ($this->cache !== null) {
            $maxAge = '?maxAge='.$this->cache['lastChange'];
        } else {
            $maxAge = '';
        };
        $proxy_host = $proxy_port = $proxy_user = $proxy_pass = '';
        if ($proxy = parse_url($this->config->get('http_proxy'))) {
            $proxy_host = @$proxy['host'];
            $proxy_port = @$proxy['port'];
            $proxy_user = @$proxy['user'];
            $proxy_pass = @$proxy['pass'];
        }        
        $c = new XML_RPC_Client('/xmlrpc.php'.$maxAge, $server_host, 80, $proxy_host, $proxy_port, $proxy_user, $proxy_pass);
        if ($username && $password) {
            $c->setCredentials($username, $password);
        }
        if ($this->config->get('verbose') >= 3) {
            $c->setDebug(1);
        }
        $r = $c->send($f);
        if (!$r) {
            return $this->raiseError("XML_RPC send failed");
        }
        $v = $r->value();
        if ($e = $r->faultCode()) {
            if ($e == $GLOBALS['XML_RPC_err']['http_error'] && strstr($r->faultString(), '304 Not Modified') !== false) {
                return $this->cache['content'];
            }
            return $this->raiseError($r->faultString(), $e);
        }

        $result = XML_RPC_decode($v);
        $this->saveCache($_args, $result);
        return $result;
    }

    // }}}

    // {{{ call_epi(method, [args...])

    function call_epi($method)
    {
        do {
            if (extension_loaded("xmlrpc")) {
                break;
            }
            if (OS_WINDOWS) {
                $ext = 'dll';
            } elseif (PHP_OS == 'HP-UX') {
                $ext = 'sl';
            } elseif (PHP_OS == 'AIX') {
                $ext = 'a';
            } else {
                $ext = 'so';
            }
            $ext = OS_WINDOWS ? 'dll' : 'so';
            @dl("xmlrpc-epi.$ext");
            if (extension_loaded("xmlrpc")) {
                break;
            }
            @dl("xmlrpc.$ext");
            if (extension_loaded("xmlrpc")) {
                break;
            }
            return $this->raiseError("unable to load xmlrpc extension");
        } while (false);
        $params = func_get_args();
        array_shift($params);
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
            $req_headers .= "Cookie: PEAR_USER=$username; PEAR_PW=$password\r\n";
            $tmp = base64_encode("$username:$password");
            $req_headers .= "Authorization: Basic $tmp\r\n";
        }
        if ($this->cache !== null) {
            $maxAge = '?maxAge='.$this->cache['lastChange'];
        } else {
            $maxAge = '';
        };
        
        if ($this->config->get('verbose') > 3) {
            print "XMLRPC REQUEST HEADERS:\n";
            var_dump($req_headers);
            print "XMLRPC REQUEST BODY:\n";
            var_dump($request);
        }
        
        fwrite($fp, ("POST /xmlrpc.php$maxAge HTTP/1.0\r\n$req_headers\r\n$request"));
        $response = '';
        $line1 = fgets($fp, 2048);
        if (!preg_match('!^HTTP/[0-9\.]+ (\d+) (.*)!', $line1, $matches)) {
            return $this->raiseError("PEAR_Remote: invalid HTTP response from XML-RPC server");
        }
        switch ($matches[1]) {
            case "200": // OK
                break;
            case "304": // Not Modified
                return $this->cache['content'];
            case "401": // Unauthorized
                if ($username && $password) {
                    return $this->raiseError("PEAR_Remote: authorization failed", 401);
                } else {
                    return $this->raiseError("PEAR_Remote: authorization required, please log in first", 401);
                }
            default:
                return $this->raiseError("PEAR_Remote: unexpected HTTP response", (int)$matches[1], null, null, "$matches[1] $matches[2]");
        }
        while (trim(fgets($fp, 2048)) != ''); // skip rest of headers
        while ($chunk = fread($fp, 10240)) {
            $response .= $chunk;
        }
        fclose($fp);
        if ($this->config->get('verbose') > 3) {
            print "XMLRPC RESPONSE:\n";
            var_dump($response);
        }
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
                    $ret = $this->raiseError(PEAR::errorMessage($ret['code']),
                                             $ret['code'], null, null,
                                             $ret['userinfo']);
                } else {
                    $ret = $this->raiseError($ret['message'], $ret['code'],
                                             null, null, $ret['userinfo']);
                }
            }
        } elseif (is_array($ret) && sizeof($ret) == 1 && is_array($ret[0]) &&
                  !empty($ret[0]['faultString']) &&
                  !empty($ret[0]['faultCode'])) {
            extract($ret[0]);
            $faultString = "XML-RPC Server Fault: " .
                 str_replace("\n", " ", $faultString);
            return $this->raiseError($faultString, $faultCode);
        }
        return $ret;
    }

    // }}}

    // {{{ _encode

    // a slightly extended version of XML_RPC_encode
    function _encode($php_val)
    {
        global $XML_RPC_Boolean, $XML_RPC_Int, $XML_RPC_Double;
        global $XML_RPC_String, $XML_RPC_Array, $XML_RPC_Struct;

        $type = gettype($php_val);
        $xmlrpcval = new XML_RPC_Value;

        switch($type) {
            case "array":
                reset($php_val);
                $firstkey = key($php_val);
                end($php_val);
                $lastkey = key($php_val);
                if ($firstkey === 0 && is_int($lastkey) &&
                    ($lastkey + 1) == count($php_val)) {
                    $is_continuous = true;
                    reset($php_val);
                    $size = count($php_val);
                    for ($expect = 0; $expect < $size; $expect++, next($php_val)) {
                        if (key($php_val) !== $expect) {
                            $is_continuous = false;
                            break;
                        }
                    }
                    if ($is_continuous) {
                        reset($php_val);
                        $arr = array();
                        while (list($k, $v) = each($php_val)) {
                            $arr[$k] = $this->_encode($v);
                        }
                        $xmlrpcval->addArray($arr);
                        break;
                    }
                }
                // fall though if not numerical and continuous
            case "object":
                $arr = array();
                while (list($k, $v) = each($php_val)) {
                    $arr[$k] = $this->_encode($v);
                }
                $xmlrpcval->addStruct($arr);
                break;
            case "integer":
                $xmlrpcval->addScalar($php_val, $XML_RPC_Int);
                break;
            case "double":
                $xmlrpcval->addScalar($php_val, $XML_RPC_Double);
                break;
            case "string":
            case "NULL":
                $xmlrpcval->addScalar($php_val, $XML_RPC_String);
                break;
            case "boolean":
                $xmlrpcval->addScalar($php_val, $XML_RPC_Boolean);
                break;
            case "unknown type":
            default:
                return null;
        }
        return $xmlrpcval;
    }

    // }}}

}

?>
