--TEST--
inet_ntop() & inet_pton() tests
--SKIPIF--
<?php
if (!function_exists("inet_ntop")) die("skip no inet_ntop()");
if (!function_exists("inet_pton")) die("skip no inet_pton()");
?>
--FILE--
<?php

$packed = chr(127) . chr(0) . chr(0) . chr(1);
var_dump(inet_ntop($packed));

$packed = chr(255) . chr(255) . chr(255) . chr(0);
var_dump(inet_ntop($packed));

var_dump(inet_ntop());
var_dump(inet_ntop(-1));
var_dump(inet_ntop(""));
var_dump(inet_ntop("blah-blah"));

var_dump(inet_pton());
var_dump(inet_pton(""));
var_dump(inet_pton(-1));
var_dump(inet_pton("abra"));

$array = array(
	"127.0.0.1",
	"66.163.161.116",
	"255.255.255.255",
	"0.0.0.0",
	);
foreach ($array as $val) {
	var_dump(bin2hex($packed = inet_pton($val)));
	var_dump(inet_ntop($packed));
}

echo "Done\n";
?>
--EXPECTF--
string(9) "127.0.0.1"
string(13) "255.255.255.0"

Warning: inet_ntop() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)

Warning: inet_pton() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
string(8) "7f000001"
string(9) "127.0.0.1"
string(8) "42a3a174"
string(14) "66.163.161.116"
string(8) "ffffffff"
string(15) "255.255.255.255"
string(8) "00000000"
string(7) "0.0.0.0"
Done
