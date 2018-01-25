<?php
/**
 * The following file shows how to bootstrap phpdbg so that you can mock specific server environments
 *
 * eval include("web-bootstrap.php")
 * exec index.php
 * compile
 * break ...
 * run
 */
if (!defined('PHPDBG_BOOTSTRAPPED'))
{
    /* define these once */
    define("PHPDBG_BOOTPATH", "/opt/php-zts/htdocs");
    define("PHPDBG_BOOTSTRAP", "index.php");
    define("PHPDBG_BOOTSTRAPPED", sprintf("/%s", PHPDBG_BOOTSTRAP));
}

/*
 * Superglobals are JIT, phpdbg will not over-write whatever you set during bootstrap
 */

$_SERVER = array
(
  'HTTP_HOST' => 'localhost',
  'HTTP_CONNECTION' => 'keep-alive',
  'HTTP_ACCEPT' => 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
  'HTTP_USER_AGENT' => 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.65 Safari/537.36',
  'HTTP_ACCEPT_ENCODING' => 'gzip,deflate,sdch',
  'HTTP_ACCEPT_LANGUAGE' => 'en-US,en;q=0.8',
  'HTTP_COOKIE' => 'tz=Europe%2FLondon; __utma=1.347100075.1384196523.1384196523.1384196523.1; __utmc=1; __utmz=1.1384196523.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none)',
  'PATH' => '/usr/local/bin:/usr/bin:/bin',
  'SERVER_SIGNATURE' => '<address>Apache/2.4.6 (Ubuntu) Server at phpdbg.com Port 80</address>',
  'SERVER_SOFTWARE' => 'Apache/2.4.6 (Ubuntu)',
  'SERVER_NAME' => 'localhost',
  'SERVER_ADDR' => '127.0.0.1',
  'SERVER_PORT' => '80',
  'REMOTE_ADDR' => '127.0.0.1',
  'DOCUMENT_ROOT' => PHPDBG_BOOTPATH,
  'REQUEST_SCHEME' => 'http',
  'CONTEXT_PREFIX' => '',
  'CONTEXT_DOCUMENT_ROOT' => PHPDBG_BOOTPATH,
  'SERVER_ADMIN' => '[no address given]',
  'SCRIPT_FILENAME' => sprintf(
    '%s/%s', PHPDBG_BOOTPATH, PHPDBG_BOOTSTRAP
  ),
  'REMOTE_PORT' => '47931',
  'GATEWAY_INTERFACE' => 'CGI/1.1',
  'SERVER_PROTOCOL' => 'HTTP/1.1',
  'REQUEST_METHOD' => 'GET',
  'QUERY_STRING' => '',
  'REQUEST_URI' => PHPDBG_BOOTSTRAPPED,
  'SCRIPT_NAME' => PHPDBG_BOOTSTRAPPED,
  'PHP_SELF' => PHPDBG_BOOTSTRAPPED,
  'REQUEST_TIME' => time(),
);

$_GET = array();
$_REQUEST = array();
$_POST = array();
$_COOKIE = array();
$_FILES = array();

chdir(PHPDBG_BOOTPATH);
