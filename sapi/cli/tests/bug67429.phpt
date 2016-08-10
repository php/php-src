--TEST--
FR #67429 (CLI server is missing some new HTTP response codes)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

foreach ([308, 426] as $code) {
  php_cli_server_start(<<<PHP
http_response_code($code);
PHP
  );

  list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
  $port = intval($port)?:80;

  $fp = fsockopen($host, $port, $errno, $errstr, 0.5);
  if (!$fp) {
    die("connect failed");
  }

  if(fwrite($fp, <<<HEADER
GET / HTTP/1.1


HEADER
  )) {
      while (!feof($fp)) {
          echo fgets($fp);
      }
  }

  fclose($fp);
}
?>
--EXPECTF--
HTTP/1.1 308 Permanent Redirect
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

HTTP/1.1 426 Upgrade Required
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

