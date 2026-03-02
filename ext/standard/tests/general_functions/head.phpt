--TEST--
header() and friends
--FILE--
<?php

$v1 = headers_sent();
$v2 = headers_list();
var_dump(header("HTTP 1.0", true, 200));

var_dump($v1);
var_dump($v2);

var_dump(header(""));
var_dump(header("", true));
var_dump(headers_sent());
var_dump(headers_list());
var_dump(header("HTTP blah"));
var_dump(header("HTTP blah", true));
var_dump(headers_sent());
var_dump(headers_list());

echo "Done\n";
?>
--EXPECTF--
NULL
bool(false)
array(0) {
}

Warning: Cannot modify header information - headers already sent by (output started at %s:%d) in %s on line %d
NULL

Warning: Cannot modify header information - headers already sent by (output started at %s:%d) in %s on line %d
NULL
bool(true)
array(0) {
}

Warning: Cannot modify header information - headers already sent by (output started at %s:%d) in %s on line %d
NULL

Warning: Cannot modify header information - headers already sent by (output started at %s:%d) in %s on line %d
NULL
bool(true)
array(0) {
}
Done
