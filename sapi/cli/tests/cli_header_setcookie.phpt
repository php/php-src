--TEST--
CLI setcookie() create header
--SKIPIF--
<?php include "skipif.inc"; ?>
--INI--
expose_php=On
--FILE--
<?php
setcookie('hi', time()+3600);

setcookie('hi');

$headers = headers_list();
print_r($headers);
?>
--EXPECTF--
Array
(
    [0] => X-Powered-By: %s
    [1] => Set-Cookie: hi=%s
    [2] => Set-Cookie: hi=deleted;%s
)
