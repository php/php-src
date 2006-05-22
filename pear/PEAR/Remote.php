<?php
/**
 * PEAR_Remote
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
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 0.1
 */

/**
 * needed for PEAR_Error
 */
require_once 'PEAR.php';
require_once 'PEAR/Config.php';

/**
 * This is a class for doing remote operations against the central
 * PEAR database.
 *
 * @nodep XML_RPC_Value
 * @nodep XML_RPC_Message
 * @nodep XML_RPC_Client
 * @category   pear
 * @package    PEAR
 * @author     Stig Bakken <ssb@php.net>
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 0.1
 */
class PEAR_Remote extends PEAR
{
    // {{{ properties

    var $config = null;
    var $cache  = null;
    /**
     * @var PEAR_Registry
     * @access private
     */
    var $_registry;

    // }}}

    // {{{ PEAR_Remote(config_object)

    function PEAR_Remote(&$config)
    {
        $this->PEAR();
        $this->config = &$config;
        $this->_registry = &$this->config->getRegistry();
    }

    // }}}
    // {{{ setRegistry()
    
    function setRegistry(&$reg)
    {
        $this->_registry = &$reg;
    }
    // }}}
    // {{{ getCache()


    function getCache($args)
    {
        $id       = md5(serialize($args));
        $cachedir = $this->config->get('cache_dir');
        $filename = $cachedir . DIRECTORY_SEPARATOR . 'xmlrpc_cache_' . $id;
        if (!file_exists($filename)) {
            return null;
        }

        $fp = fopen($filename, 'rb');
        if (!$fp) {
            return null;
        }
        if (function_exists('file_get_contents')) {
            fclose($fp);
            $content = file_get_contents($filename);
        } else {
            $content  = fread($fp, filesize($filename));
            fclose($fp);
        }
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
            System::mkdir(array('-p', $cachedir));
        }
        $filename = $cachedir.'/xmlrpc_cache_'.$id;

        $fp = @fopen($filename, "wb");
        if ($fp) {
            fwrite($fp, serialize($data));
            fclose($fp);
        }
    }

    // }}}

    // {{{ clearCache()

    function clearCache($method, $args)
    {
        array_unshift($args, $method);
        array_unshift($args, $this->config->get('default_channel')); // cache by channel
        $id       = md5(serialize($args));
        $cachedir = $this->config->get('cache_dir');
        $filename = $cachedir.'/xmlrpc_cache_'.$id;
        if (file_exists($filename)) {
            @unlink($filename);
        }
    }

    // }}}
    // {{{ call(method, [args...])

    function call($method)
    {
        $_args = $args = func_get_args();

        $server_channel = $this->config->get('default_channel');
        $channel = $this->_registry->getChannel($server_channel);
        if (!PEAR::isError($channel)) {
            $mirror = $this->config->get('preferred_mirror');
            if ($channel->getMirror($mirror)) {
                if ($channel->supports('xmlrpc', $method, $mirror)) {
                    $server_channel = $server_host = $mirror; // use the preferred mirror
                    $server_port = $channel->getPort($mirror);
                } elseif (!$channel->supports('xmlrpc', $method)) {
                    return $this->raiseError("Channel $server_channel does not " .
                        "support xml-rpc method $method");
                }
            }
            if (!isset($server_host)) {
                if (!$channel->supports('xmlrpc', $method)) {
                    return $this->raiseError("Channel $server_channel does not support " .
                        "xml-rpc method $method");
                } else {
                    $server_host = $server_channel;
                    $server_port = $channel->getPort();
                }
            }
        } else {
            return $this->raiseError("Unknown channel '$server_channel'");
        }

        array_unshift($_args, $server_channel); // cache by channel
        $this->cache = $this->getCache($_args);
        $cachettl = $this->config->get('cache_ttl');
        // If cache is newer than $cachettl seconds, we use the cache!
        if ($this->cache !== null && $this->cache['age'] < $cachettl) {
            return $this->cache['content'];
        }
        if (extension_loaded("xmlrpc")) {
            $result = call_user_func_array(array(&$this, 'call_epi'), $args);
            if (!PEAR::isError($result)) {
                $this->saveCache($_args, $result);
            }
            return $result;
        } elseif (!@include_once 'XML/RPC.php') {
            return $this->raiseError("For this remote PEAR operation you need to load the xmlrpc extension or install XML_RPC");
        }

        array_shift($args);
        $username = $this->config->get('username');
        $password = $this->config->get('password');
        $eargs = array();
        foreach($args as $arg) {
            $eargs[] = $this->_encode($arg);
        }
        $f = new XML_RPC_Message($method, $eargs);
        if ($this->cache !== null) {
            $maxAge = '?maxAge='.$this->cache['lastChange'];
        } else {
            $maxAge = '';
        }
        $proxy_host = $proxy_port = $proxy_user = $proxy_pass = '';
        if ($proxy = parse_url($this->config->get('http_proxy'))) {
            $proxy_host = @$proxy['host'];
            if (isset($proxy['scheme']) && $proxy['scheme'] == 'https') {
                $proxy_host = 'https://' . $proxy_host;
            }
            $proxy_port = @$proxy['port'];
            $proxy_user = @urldecode(@$proxy['user']);
            $proxy_pass = @urldecode(@$proxy['pass']);
        }
        $shost = $server_host;
        if ($channel->getSSL()) {
            $shost = "https://$shost";
        }
        $c = new XML_RPC_Client('/' . $channel->getPath('xmlrpc')
            . $maxAge, $shost, $server_port, $proxy_host, $proxy_port,
            $proxy_user, $proxy_pass);
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
        $server_channel = $this->config->get('default_channel');
        $channel = $this->_registry->getChannel($server_channel);
        if (!PEAR::isError($channel)) {
            $mirror = $this->config->get('preferred_mirror');
            if ($channel->getMirror($mirror)) {
                if ($channel->supports('xmlrpc', $method, $mirror)) {
                    $server_channel = $server_host = $mirror; // use the preferred mirror
                    $server_port = $channel->getPort($mirror);
                } elseif (!$channel->supports('xmlrpc', $method)) {
                    return $this->raiseError("Channel $server_channel does not " .
                        "support xml-rpc method $method");
                }
            }
            if (!isset($server_host)) {
                if (!$channel->supports('xmlrpc', $method)) {
                    return $this->raiseError("Channel $server_channel does not support " .
                        "xml-rpc method $method");
                } else {
                    $server_host = $server_channel;
                    $server_port = $channel->getPort();
                }
            }
        } else {
            return $this->raiseError("Unknown channel '$server_channel'");
        }
        $params = func_get_args();
        array_shift($params);
        $method = str_replace("_", ".", $method);
        $request = xmlrpc_encode_request($method, $params);
        if ($http_proxy = $this->config->get('http_proxy')) {
            $proxy = parse_url($http_proxy);
            $proxy_host = $proxy_port = $proxy_user = $proxy_pass = '';
            $proxy_host = @$proxy['host'];
            if (isset($proxy['scheme']) && $proxy['scheme'] == 'https') {
                $proxy_host = 'ssl://' . $proxy_host;
            }
            $proxy_port = @$proxy['port'];
            $proxy_user = @urldecode(@$proxy['user']);
            $proxy_pass = @urldecode(@$proxy['pass']);
            $fp = @fsockopen($proxy_host, $proxy_port);
            $use_proxy = true;
            if ($channel->getSSL()) {
                $server_host = "https://$server_host";
            }
        } else {
            $use_proxy = false;
            $ssl = $channel->getSSL();
            $fp = @fsockopen(($ssl ? 'ssl://' : '') . $server_host, $server_port);
            if (!$fp) {
                $server_host = "$ssl$server_host"; // for error-reporting
            }
        }
        if (!$fp && $http_proxy) {
            return $this->raiseError("PEAR_Remote::call: fsockopen(`$proxy_host', $proxy_port) failed");
        } elseif (!$fp) {
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
        }

        if ($use_proxy && $proxy_host != '' && $proxy_user != '') {
            $req_headers .= 'Proxy-Authorization: Basic '
                .base64_encode($proxy_user.':'.$proxy_pass)
                ."\r\n";
        }

        if ($this->config->get('verbose') > 3) {
            print "XMLRPC REQUEST HEADERS:\n";
            var_dump($req_headers);
            print "XMLRPC REQUEST BODY:\n";
            var_dump($request);
        }

        if ($use_proxy && $proxy_host != '') {
            $post_string = "POST http://".$server_host;
            if ($proxy_port > '') {
                $post_string .= ':'.$server_port;
            }
        } else {
            $post_string = "POST ";
        }

        $path = '/' . $channel->getPath('xmlrpc');
        fwrite($fp, ($post_string . $path . "$maxAge HTTP/1.0\r\n$req_headers\r\n$request"));
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
                    return $this->raiseError("PEAR_Remote ($server_host:$server_port) " .
                        ": authorization failed", 401);
                } else {
                    return $this->raiseError("PEAR_Remote ($server_host:$server_port) " .
                        ": authorization required, please log in first", 401);
                }
            default:
                return $this->raiseError("PEAR_Remote ($server_host:$server_port) : " .
                    "unexpected HTTP response", (int)$matches[1], null, null,
                    "$matches[1] $matches[2]");
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
        } elseif (is_array($ret) && sizeof($ret) == 1 && isset($ret[0])
                  && is_array($ret[0]) &&
                  !empty($ret[0]['faultString']) &&
                  !empty($ret[0]['faultCode'])) {
            extract($ret[0]);
            $faultString = "XML-RPC Server Fault: " .
                 str_replace("\n", " ", $faultString);
            return $this->raiseError($faultString, $faultCode);
        } elseif (is_array($ret) && sizeof($ret) == 2 && !empty($ret['faultString']) &&
              !empty($ret['faultCode'])) {
            extract($ret);
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
                reset($php_val);
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
