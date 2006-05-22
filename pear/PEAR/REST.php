<?php
/**
 * PEAR_REST
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
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/PEAR
 * @since      File available since Release 1.4.0a1
 */

/**
 * For downloading xml files
 */
require_once 'PEAR.php';
require_once 'PEAR/XMLParser.php';

/**
 * Intelligently retrieve data, following hyperlinks if necessary, and re-directing
 * as well
 * @category   pear
 * @package    PEAR
 * @author     Greg Beaver <cellog@php.net>
 * @copyright  1997-2005 The PHP Group
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @version    Release: @package_version@
 * @link       http://pear.php.net/package/PEAR
 * @since      Class available since Release 1.4.0a1
 */
class PEAR_REST
{
    var $config;
    var $_options;
    function PEAR_REST(&$config, $options = array())
    {
        $this->config = &$config;
        $this->_options = $options;
    }

    /**
     * Retrieve REST data, but always retrieve the local cache if it is available.
     *
     * This is useful for elements that should never change, such as information on a particular
     * release
     * @param string full URL to this resource
     * @param array|false contents of the accept-encoding header
     * @param boolean     if true, xml will be returned as a string, otherwise, xml will be
     *                    parsed using PEAR_XMLParser
     * @return string|array
     */
    function retrieveCacheFirst($url, $accept = false, $forcestring = false)
    {
        $cachefile = $this->config->get('cache_dir') . DIRECTORY_SEPARATOR .
            md5($url) . 'rest.cachefile';
        if (@file_exists($cachefile)) {
            return unserialize(implode('', file($cachefile)));
        }
        return $this->retrieveData($url, $accept, $forcestring);
    }

    /**
     * Retrieve a remote REST resource
     * @param string full URL to this resource
     * @param array|false contents of the accept-encoding header
     * @param boolean     if true, xml will be returned as a string, otherwise, xml will be
     *                    parsed using PEAR_XMLParser
     * @return string|array
     */
    function retrieveData($url, $accept = false, $forcestring = false)
    {
        $cacheId = $this->getCacheId($url);
        if ($ret = $this->useLocalCache($url, $cacheId)) {
            return $ret;
        }
        if (!isset($this->_options['offline'])) {
            $trieddownload = true;
            $file = $this->downloadHttp($url, $cacheId ? $cacheId['lastChange'] : false, $accept);
        } else {
            $trieddownload = false;
            $file = false;
        }
        if (PEAR::isError($file)) {
            if ($file->getCode() == -9276) {
                $trieddownload = false;
                $file = false; // use local copy if available on socket connect error
            } else {
                return $file;
            }
        }
        if (!$file) {
            $ret = $this->getCache($url);
            if (!PEAR::isError($ret) && $trieddownload) {
                // reset the age of the cache if the server says it was unmodified
                $this->saveCache($url, $ret, null, true, $cacheId);
            }
            return $ret;
        }
        if (is_array($file)) {
            $headers = $file[2];
            $lastmodified = $file[1];
            $content = $file[0];
        } else {
            $content = $file;
            $lastmodified = false;
            $headers = array();
        }
        if ($forcestring) {
            $this->saveCache($url, $content, $lastmodified, false, $cacheId);
            return $content;
        }
        if (isset($headers['content-type'])) {
            switch ($headers['content-type']) {
                case 'text/xml' :
                case 'application/xml' :
                    $parser = new PEAR_XMLParser;
                    PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
                    $err = $parser->parse($content);
                    PEAR::popErrorHandling();
                    if (PEAR::isError($err)) {
                        return PEAR::raiseError('Invalid xml downloaded from "' . $url . '": ' .
                            $err->getMessage());
                    }
                    $content = $parser->getData();
                case 'text/html' :
                default :
                    // use it as a string
            }
        } else {
            // assume XML
            $parser = new PEAR_XMLParser;
            $parser->parse($content);
            $content = $parser->getData();
        }
        $this->saveCache($url, $content, $lastmodified, false, $cacheId);
        return $content;
    }

    function useLocalCache($url, $cacheid = null)
    {
        if ($cacheid === null) {
            $cacheidfile = $this->config->get('cache_dir') . DIRECTORY_SEPARATOR .
                md5($url) . 'rest.cacheid';
            if (@file_exists($cacheidfile)) {
                $cacheid = unserialize(implode('', file($cacheidfile)));
            } else {
                return false;
            }
        }
        $cachettl = $this->config->get('cache_ttl');
        // If cache is newer than $cachettl seconds, we use the cache!
        if (time() - $cacheid['age'] < $cachettl) {
            return $this->getCache($url);
        }
        return false;
    }

    function getCacheId($url)
    {
        $cacheidfile = $this->config->get('cache_dir') . DIRECTORY_SEPARATOR .
            md5($url) . 'rest.cacheid';
        if (@file_exists($cacheidfile)) {
            $ret = unserialize(implode('', file($cacheidfile)));
            return $ret;
        } else {
            return false;
        }
    }

    function getCache($url)
    {
        $cachefile = $this->config->get('cache_dir') . DIRECTORY_SEPARATOR .
            md5($url) . 'rest.cachefile';
        if (@file_exists($cachefile)) {
            return unserialize(implode('', file($cachefile)));
        } else {
            return PEAR::raiseError('No cached content available for "' . $url . '"');
        }
    }

    /**
     * @param string full URL to REST resource
     * @param string original contents of the REST resource
     * @param array  HTTP Last-Modified and ETag headers
     * @param bool   if true, then the cache id file should be regenerated to
     *               trigger a new time-to-live value
     */
    function saveCache($url, $contents, $lastmodified, $nochange = false, $cacheid = null)
    {
        $cacheidfile = $this->config->get('cache_dir') . DIRECTORY_SEPARATOR .
            md5($url) . 'rest.cacheid';
        $cachefile = $this->config->get('cache_dir') . DIRECTORY_SEPARATOR .
            md5($url) . 'rest.cachefile';
        if ($cacheid === null && $nochange) {
            $cacheid = unserialize(implode('', file($cacheidfile)));
        }

        $fp = @fopen($cacheidfile, 'wb');
        if (!$fp) {
            $cache_dir = $this->config->get('cache_dir');
            if (!is_dir($cache_dir)) {
                System::mkdir(array('-p', $cache_dir));
                $fp = @fopen($cacheidfile, 'wb');
                if (!$fp) {
                    return false;
                }
            } else {
                return false;
            }
        }

        if ($nochange) {
            fwrite($fp, serialize(array(
                'age'        => time(),
                'lastChange' => $cacheid['lastChange'],
                )));
            fclose($fp);
            return true;
        } else {
            fwrite($fp, serialize(array(
                'age'        => time(),
                'lastChange' => $lastmodified,
                )));
        }
        fclose($fp);
        $fp = @fopen($cachefile, 'wb');
        if (!$fp) {
            @unlink($cacheidfile);
            return false;
        }
        fwrite($fp, serialize($contents));
        fclose($fp);
        return true;
    }

    /**
     * Efficiently Download a file through HTTP.  Returns downloaded file as a string in-memory
     * This is best used for small files
     *
     * If an HTTP proxy has been configured (http_proxy PEAR_Config
     * setting), the proxy will be used.
     *
     * @param string  $url       the URL to download
     * @param string  $save_dir  directory to save file in
     * @param false|string|array $lastmodified header values to check against for caching
     *                           use false to return the header values from this download
     * @param false|array $accept Accept headers to send
     * @return string|array  Returns the contents of the downloaded file or a PEAR
     *                       error on failure.  If the error is caused by
     *                       socket-related errors, the error object will
     *                       have the fsockopen error code available through
     *                       getCode().  If caching is requested, then return the header
     *                       values.
     *
     * @access public
     */
    function downloadHttp($url, $lastmodified = null, $accept = false)
    {
        $info = parse_url($url);
        if (!isset($info['scheme']) || !in_array($info['scheme'], array('http', 'https'))) {
            return PEAR::raiseError('Cannot download non-http URL "' . $url . '"');
        }
        if (!isset($info['host'])) {
            return PEAR::raiseError('Cannot download from non-URL "' . $url . '"');
        } else {
            $host = $info['host'];
            if (!array_key_exists('port', $info)) {
                $info['port'] = null;
            }
            if (!array_key_exists('path', $info)) {
                $info['path'] = null;
            }
            $port = $info['port'];
            $path = $info['path'];
        }
        $proxy_host = $proxy_port = $proxy_user = $proxy_pass = '';
        if ($this->config->get('http_proxy')&& 
              $proxy = parse_url($this->config->get('http_proxy'))) {
            $proxy_host = @$proxy['host'];
            if (isset($proxy['scheme']) && $proxy['scheme'] == 'https') {
                $proxy_host = 'ssl://' . $proxy_host;
            }
            $proxy_port = @$proxy['port'];
            $proxy_user = @$proxy['user'];
            $proxy_pass = @$proxy['pass'];

            if ($proxy_port == '') {
                $proxy_port = 8080;
            }
        }
        if (empty($port)) {
            if (isset($info['scheme']) && $info['scheme'] == 'https') {
                $port = 443;
            } else {
                $port = 80;
            }
        }
        If (isset($proxy['host'])) {
            $request = "GET $url HTTP/1.1\r\n";
        } else {
            $request = "GET $path HTTP/1.1\r\n";
        }

        $ifmodifiedsince = '';
        if (is_array($lastmodified)) {
            if (isset($lastmodified['Last-Modified'])) {
                $ifmodifiedsince = 'If-Modified-Since: ' . $lastmodified['Last-Modified'] . "\r\n";
            }
            if (isset($lastmodified['ETag'])) {
                $ifmodifiedsince .= "If-None-Match: $lastmodified[ETag]\r\n";
            }
        } else {
            $ifmodifiedsince = ($lastmodified ? "If-Modified-Since: $lastmodified\r\n" : '');
        }
        $request .= "Host: $host:$port\r\n" . $ifmodifiedsince .
            "User-Agent: PEAR/@package_version@/PHP/" . PHP_VERSION . "\r\n";
        $username = $this->config->get('username');
        $password = $this->config->get('password');
        if ($username && $password) {
            $tmp = base64_encode("$username:$password");
            $request .= "Authorization: Basic $tmp\r\n";
        }
        if ($proxy_host != '' && $proxy_user != '') {
            $request .= 'Proxy-Authorization: Basic ' .
                base64_encode($proxy_user . ':' . $proxy_pass) . "\r\n";
        }
        if ($accept) {
            $request .= 'Accept: ' . implode(', ', $accept) . "\r\n";
        }
        $request .= "Connection: close\r\n";
        $request .= "\r\n";
        if ($proxy_host != '') {
            $fp = @fsockopen($proxy_host, $proxy_port, $errno, $errstr, 15);
            if (!$fp) {
                return PEAR::raiseError("Connection to `$proxy_host:$proxy_port' failed: $errstr",
                    -9276);
            }
        } else {
            if (isset($info['scheme']) && $info['scheme'] == 'https') {
                $host = 'ssl://' . $host;
            }
            $fp = @fsockopen($host, $port, $errno, $errstr);
            if (!$fp) {
                return PEAR::raiseError("Connection to `$host:$port' failed: $errstr", $errno);
            }
        }
        fwrite($fp, $request);
        $headers = array();
        while (trim($line = fgets($fp, 1024))) {
            if (preg_match('/^([^:]+):\s+(.*)\s*$/', $line, $matches)) {
                $headers[strtolower($matches[1])] = trim($matches[2]);
            } elseif (preg_match('|^HTTP/1.[01] ([0-9]{3}) |', $line, $matches)) {
                if ($matches[1] == 304 && ($lastmodified || ($lastmodified === false))) {
                    return false;
                }
                if ($matches[1] != 200) {
                    return PEAR::raiseError("File http://$host:$port$path not valid (received: $line)", (int) $matches[1]);
                }
            }
        }
        if (isset($headers['content-length'])) {
            $length = $headers['content-length'];
        } else {
            $length = -1;
        }
        $data = '';
        while ($chunk = @fread($fp, 8192)) {
            $data .= $chunk;
        }
        fclose($fp);
        if ($lastmodified === false || $lastmodified) {
            if (isset($headers['etag'])) {
                $lastmodified = array('ETag' => $headers['etag']);
            }
            if (isset($headers['last-modified'])) {
                if (is_array($lastmodified)) {
                    $lastmodified['Last-Modified'] = $headers['last-modified'];
                } else {
                    $lastmodified = $headers['last-modified'];
                }
            }
            return array($data, $lastmodified, $headers);
        }
        return $data;
    }
}
?>