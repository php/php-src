<?php
require_once 'XML/RPC.php';
require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'PEAR_test_mock_pearweb.php.inc';
$GLOBALS['pearweb'] = new PEAR_test_mock_pearweb;
class test_XML_RPC_Client extends XML_RPC_Client {
    function test_XML_RPC_Client()
    {
        $this->_fakepearweb = &$GLOBALS['pearweb'];
    }

    function sendPayloadHTTP10($msg, $server, $port, $timeout=0,
                               $username="", $password="")
    {
        // Only create the payload if it was not created previously
        if(empty($msg->payload)) $msg->createPayload();

        $resp = $this->_fakepearweb->receiveXmlrpc($msg->payload);
        
        $resp=$msg->parseResponse($resp);
        return $resp;
    }
}
require_once 'PEAR/Remote.php';

class test_PEAR_Remote extends PEAR_Remote {
    var $_pearweb;
    var $_fakepearweb = true;
    var $_simulateEpi = true;
    function test_PEAR_Remote($config)
    {
        $pearweb = &$GLOBALS['pearweb'];
        include dirname(__FILE__) . DIRECTORY_SEPARATOR . 'download_test.config.inc';
        $this->_pearweb = $pearweb;
        parent::PEAR_Remote($config);
        $this->_pearweb->setRemote($this);
    }
    
    function parentcall()
    {
        $args = func_get_args();
        $method = array_shift($args);
        switch (count($args)) {
            case 0:
                $result = PEAR_Remote::call_epi($method);
            break;
            case 1:
                $result = PEAR_Remote::call_epi($method, $args[0]);
            break;
            case 2:
                $result = PEAR_Remote::call_epi($method, $args[0], $args[1]);
            break;
            case 3:
                $result = PEAR_Remote::call_epi($method, $args[0], $args[1], $args[2]);
            break;
            case 4:
                $result = PEAR_Remote::call_epi($method, $args[0], $args[1], $args[2], $args[3]);
            break;
            case 5:
                $result = PEAR_Remote::call_epi($method, $args[0], $args[1], $args[2], $args[3], $args[4]);
            break;
            case 6:
                $result = PEAR_Remote::call_epi($method, $args[0], $args[1], $args[2], $args[3], $args[4], $args[5]);
            break;
        }
        return $result;
    }

    function call($method)
    {
        $_args = $args = func_get_args();

        $server_channel = $this->config->get('default_channel');
        $channel = $this->_registry->getChannel($server_channel);
        if ($channel) {
            if (!$channel->supports('xml-rpc', $method)) {
                // check for channel.list, which is implicitly supported for the PEAR channel
                if (!(strtolower($server_channel) == 'pear' && $method == 'channel.list')) {
                    return $this->raiseError("Channel $server_channel does not support xml-rpc method $method");
                }
            }
        }

        array_unshift($_args, $channel); // cache by channel
        $this->cache = $this->getCache($_args);
        $cachettl = $this->config->get('cache_ttl');
        // If cache is newer than $cachettl seconds, we use the cache!
        if ($this->cache !== null && $this->cache['age'] < $cachettl) {
            return $this->cache['content'];
        };
        if ($this->_simulateEpi) {
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
        $server_host = $this->_registry->channelInfo($server_channel, 'server');
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
        $c = new test_XML_RPC_Client('/xmlrpc.php'.$maxAge, $server_host, 80, $proxy_host, $proxy_port, $proxy_user, $proxy_pass);
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

    function call_epi($method)
    {
        $args = func_get_args();
        array_shift($args);
        if ($this->_fakepearweb) {
            if (count($args)) {
                $result = $this->_pearweb->call($method, $args);
            } else {
                $result = $this->_pearweb->call($method);
            }
        }/* else {
            switch (count($args)) {
                case 0:
                    $result = parent::call($method);
                break;
                case 1:
                    $result = parent::call($method, $args[0]);
                break;
                case 2:
                    $result = parent::call($method, $args[0], $args[1]);
                break;
                case 3:
                    $result = parent::call($method, $args[0], $args[1], $args[2]);
                break;
                case 4:
                    $result = parent::call($method, $args[0], $args[1], $args[2], $args[3]);
                break;
                case 5:
                    $result = parent::call($method, $args[0], $args[1], $args[2], $args[3], $args[4]);
                break;
                case 6:
                    $result = parent::call($method, $args[0], $args[1], $args[2], $args[3], $args[4], $args[5]);
                break;
            }
        }*/
        if (PEAR::isError($result)) {
            return $result;
        }
        return $result;
    }
}

require_once 'PEAR/Installer.php';
class test_PEAR_Installer extends PEAR_Installer {
    function download($packages, $options, &$config, &$installpackages,
                      &$errors, $installed = false, $willinstall = false, $state = false)
    {
        // trickiness: initialize here
        $this->PEAR_Downloader($this->ui, $options, $config);
        $this->_remote = &new test_PEAR_Remote($config);
        $ret = PEAR_Downloader::download($packages);
        $errors = $this->getErrorMsgs();
        $installpackages = $this->getDownloadedPackages();
        trigger_error("PEAR Warning: PEAR_Installer::download() is deprecated " .
                      "in favor of PEAR_Downloader class", E_USER_WARNING);
        return $ret;
    }
    
    function downloadHttp($url, &$ui, $save_dir = '.', $callback = null)
    {
//        return parent::downloadHttp($url, $ui, $save_dir, $callback);
        if ($callback) {
            call_user_func($callback, 'setup', array(&$ui));
        }
        if (preg_match('!^http://([^/:?#]*)(:(\d+))?(/.*)!', $url, $matches)) {
            list(,$host,,$port,$path) = $matches;
        }
        if (isset($this)) {
            $config = &$this->config;
        } else {
            $config = &PEAR_Config::singleton();
        }
        $proxy_host = $proxy_port = $proxy_user = $proxy_pass = '';
        if ($proxy = parse_url($config->get('http_proxy'))) {
            $proxy_host = @$proxy['host'];
            $proxy_port = @$proxy['port'];
            $proxy_user = @$proxy['user'];
            $proxy_pass = @$proxy['pass'];

            if ($proxy_port == '') {
                $proxy_port = 8080;
            }
            if ($callback) {
                call_user_func($callback, 'message', "Using HTTP proxy $host:$port");
            }
        }
        if (empty($port)) {
            $port = 80;
        }
        // use _pearweb to get file
        $retrieved = explode("\n", $this->_remote->_pearweb->receiveHttp($url));
        $headers = array();
        $line = array_shift($retrieved);
        while (strlen(trim($line))) {
            if (preg_match('/^([^:]+):\s+(.*)\s*$/', $line, $matches)) {
                $headers[strtolower($matches[1])] = trim($matches[2]);
            } elseif (preg_match('|^HTTP/1.[01] ([0-9]{3}) |', $line, $matches)) {
                if ($matches[1] != 200) {
                    return PEAR::raiseError("File http://$host:$port$path not valid (received: $line)");
                }
            }
            $line = array_shift($retrieved);
        }
        $retrieved = join("\n", $retrieved);
        if (isset($headers['content-disposition']) &&
            preg_match('/\sfilename=\"([^;]*\S)\"\s*(;|$)/', $headers['content-disposition'], $matches)) {
            $save_as = basename($matches[1]);
        } else {
            $save_as = basename($url);
        }
        if ($callback) {
            $tmp = call_user_func($callback, 'saveas', $save_as);
            if ($tmp) {
                $save_as = $tmp;
            }
        }
        $dest_file = $save_dir . DIRECTORY_SEPARATOR . $save_as;
        if (!$wp = @fopen($dest_file, 'wb')) {
            fclose($fp);
            if ($callback) {
                call_user_func($callback, 'writefailed', array($dest_file, $php_errormsg));
            }
            return PEAR::raiseError("could not open $dest_file for writing");
        }
        if (isset($headers['content-length'])) {
            $length = $headers['content-length'];
        } else {
            $length = -1;
        }
        $bytes = 0;
        if ($callback) {
            call_user_func($callback, 'start', array(basename($dest_file), $length));
        }
        $start = 0;
        while ($start < strlen($retrieved) - 1) {
            $data = substr($retrieved, $start, 1024);
            $start += 1024;
            $bytes += strlen($data);
            if ($callback) {
                call_user_func($callback, 'bytesread', $bytes);
            }
            if (!@fwrite($wp, $data)) {
                if ($callback) {
                    call_user_func($callback, 'writefailed', array($dest_file, $php_errormsg));
                }
                return PEAR::raiseError("$dest_file: write failed ($php_errormsg)");
            }
        }
        fclose($wp);
        if ($callback) {
            call_user_func($callback, 'done', $bytes);
        }
        return $dest_file;
    }
}

require_once 'PEAR/Downloader.php';
class test_PEAR_Downloader extends PEAR_Downloader {
    function test_PEAR_Downloader(&$ui, $options, &$config)
    {
        parent::PEAR_Downloader($ui, $options, $config);
        $this->_remote = new test_PEAR_Remote($config);
    }
    
    function downloadHttp($url, &$ui, $save_dir = '.', $callback = null)
    {
//        return parent::downloadHttp($url, $ui, $save_dir, $callback);
        if ($callback) {
            call_user_func($callback, 'setup', array(&$ui));
        }
        if (preg_match('!^http://([^/:?#]*)(:(\d+))?(/.*)!', $url, $matches)) {
            list(,$host,,$port,$path) = $matches;
        }
        if (isset($this)) {
            $config = &$this->config;
        } else {
            $config = &PEAR_Config::singleton();
        }
        $proxy_host = $proxy_port = $proxy_user = $proxy_pass = '';
        if ($proxy = parse_url($config->get('http_proxy'))) {
            $proxy_host = @$proxy['host'];
            $proxy_port = @$proxy['port'];
            $proxy_user = @$proxy['user'];
            $proxy_pass = @$proxy['pass'];

            if ($proxy_port == '') {
                $proxy_port = 8080;
            }
            if ($callback) {
                call_user_func($callback, 'message', "Using HTTP proxy $host:$port");
            }
        }
        if (empty($port)) {
            $port = 80;
        }
        // use _pearweb to get file
        $retrieved = explode("\n", $this->_remote->_pearweb->receiveHttp($url));
        $headers = array();
        $line = array_shift($retrieved);
        while (strlen(trim($line))) {
            if (preg_match('/^([^:]+):\s+(.*)\s*$/', $line, $matches)) {
                $headers[strtolower($matches[1])] = trim($matches[2]);
            } elseif (preg_match('|^HTTP/1.[01] ([0-9]{3}) |', $line, $matches)) {
                if ($matches[1] != 200) {
                    return PEAR::raiseError("File http://$host:$port$path not valid (received: $line)");
                }
            }
            $line = array_shift($retrieved);
        }
        $retrieved = join("\n", $retrieved);
        if (isset($headers['content-disposition']) &&
            preg_match('/\sfilename=\"([^;]*\S)\"\s*(;|$)/', $headers['content-disposition'], $matches)) {
            $save_as = basename($matches[1]);
        } else {
            $save_as = basename($url);
        }
        if ($callback) {
            $tmp = call_user_func($callback, 'saveas', $save_as);
            if ($tmp) {
                $save_as = $tmp;
            }
        }
        $dest_file = $save_dir . DIRECTORY_SEPARATOR . $save_as;
        if (!$wp = @fopen($dest_file, 'wb')) {
            fclose($fp);
            if ($callback) {
                call_user_func($callback, 'writefailed', array($dest_file, $php_errormsg));
            }
            return PEAR::raiseError("could not open $dest_file for writing");
        }
        if (isset($headers['content-length'])) {
            $length = $headers['content-length'];
        } else {
            $length = -1;
        }
        $bytes = 0;
        if ($callback) {
            call_user_func($callback, 'start', array(basename($dest_file), $length));
        }
        $start = 0;
        while ($start < strlen($retrieved) - 1) {
            $data = substr($retrieved, $start, 1024);
            $start += 1024;
            $bytes += strlen($data);
            if ($callback) {
                call_user_func($callback, 'bytesread', $bytes);
            }
            if (!@fwrite($wp, $data)) {
                if ($callback) {
                    call_user_func($callback, 'writefailed', array($dest_file, $php_errormsg));
                }
                return PEAR::raiseError("$dest_file: write failed ($php_errormsg)");
            }
        }
        fclose($wp);
        if ($callback) {
            call_user_func($callback, 'done', $bytes);
        }
        return $dest_file;
    }
}
?>