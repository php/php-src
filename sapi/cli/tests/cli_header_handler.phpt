--TEST--
CLI header(), header_remove() and headers_list()
--SKIPIF--
<?php include "skipif.inc" ?>
--INI--
expose_php=On
--FILE--
<?php
header("A: first");
header("A: second", TRUE);
$headers1 = headers_list();
header("A: third", FALSE);
$headers2 = headers_list();
header_remove("A");
$headers3 = headers_list();
print_r($headers1);
print_r($headers2);
print_r($headers3);
?>
--EXPECTF--
Array
(
    [0] => X-Powered-By: %s
    [1] => A: second
)
Array
(
    [0] => X-Powered-By: %s
    [1] => A: second
    [2] => A: third
)
Array
(
    [0] => X-Powered-By: %s
)
