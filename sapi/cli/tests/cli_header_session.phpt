--TEST--
CLI session create header
--SKIPIF--
<?php include "skipif.inc"; ?>
--INI--
expose_php=On
--FILE--
<?php
session_name("foo");
session_id('bar');
session_start();

$headers = headers_list();
print_r($headers);
?>
--EXPECTF--
Array
(
    [0] => X-Powered-By: %s
    [1] => Set-Cookie: foo=bar; path=/
    [2] => Expires: %s
    [3] => Cache-Control: no-store, no-cache, must-revalidate
    [4] => Pragma: no-cache
)
