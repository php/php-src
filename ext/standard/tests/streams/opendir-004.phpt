--TEST--
opendir() with 'ftps://' stream.
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--FILE--
<?php

$ssl=true;
require __DIR__ . "/../../../ftp/tests/server.inc";

$path="ftps://127.0.0.1:" . $port."/";

$context = stream_context_create(array('ssl' => array('cafile' =>  __DIR__ . '/../../../ftp/tests/cert.pem')));

$ds=opendir($path, $context);
var_dump($ds);
while ($fn=readdir($ds)) {
      var_dump($fn);
}
?>
--EXPECTF--
resource(%d) of type (stream)
string(5) "file1"
string(5) "file1"
string(3) "fil"
string(4) "b0rk"
