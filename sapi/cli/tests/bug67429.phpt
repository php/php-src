--TEST--
FR #67429 (CLI server is missing some new HTTP response codes)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

// This creates a new server for each response code
foreach ([308, 426] as $code) {
  $proc_handle = php_cli_server_start(<<<PHP
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
  // Shutdown the servers or another server may not be able to start
  // because of the this server still being bound to the port

  php_cli_server_stop($proc_handle);
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
