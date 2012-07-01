--TEST--
Bug #54058 (json_last_error() invalid UTF-8 produces wrong error)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$bad_utf8 = quoted_printable_decode('=B0');

json_encode($bad_utf8);
var_dump(json_last_error());

$a = new stdclass;
$a->foo = quoted_printable_decode('=B0');
json_encode($a);
var_dump(json_last_error());

$b = new stdclass;
$b->foo = $bad_utf8;
$b->bar = 1;
json_encode($b);
var_dump(json_last_error());

$c = array(
    'foo' => $bad_utf8,
    'bar' => 1
);
json_encode($c);
var_dump(json_last_error());
?>
--EXPECTF--
int(5)
int(5)
int(5)
int(5)
