--TEST--
inet_ntop() & inet_pton() tests
--SKIPIF--
<?php 
if (!function_exists("inet_ntop")) die("skip no inet_ntop()"); 
if (!function_exists("inet_pton")) die("skip no inet_pton()"); 
?>
--FILE--
<?php

$packed = pack("CCCC", 127, 0, 0, 1);
var_dump(inet_ntop((binary)$packed));

$packed = pack("CCCC", 255, 255, 255, 0);
var_dump(inet_ntop((binary)$packed));

var_dump(inet_ntop());
var_dump(inet_ntop(-1));
var_dump(inet_ntop(b""));
var_dump(inet_ntop(b"blah-blah"));

var_dump(inet_pton());
var_dump(inet_pton(b""));
var_dump(inet_pton(-1));
var_dump(inet_pton(b"abra"));

$array = array(
	b"127.0.0.1",
	b"66.163.161.116",
	b"255.255.255.255",
	b"0.0.0.0",
	);
foreach ($array as $val) {
	var_dump($packed = inet_pton($val));
	var_dump(inet_ntop($packed));
}

echo "Done\n";
?>
--EXPECTF--
unicode(9) "127.0.0.1"
unicode(13) "255.255.255.0"

Warning: inet_ntop() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: inet_ntop(): Invalid in_addr value in %s on line %d
bool(false)

Warning: inet_ntop(): Invalid in_addr value in %s on line %d
bool(false)

Warning: inet_ntop(): Invalid in_addr value in %s on line %d
bool(false)

Warning: inet_pton() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: inet_pton(): Unrecognized address  in %s on line %d
bool(false)

Warning: inet_pton(): Unrecognized address -1 in %s on line %d
bool(false)

Warning: inet_pton(): Unrecognized address abra in %s on line %d
bool(false)
string(4) "%s"
unicode(9) "127.0.0.1"
string(4) "%s"
unicode(14) "66.163.161.116"
string(4) "%s"
unicode(15) "255.255.255.255"
string(4) "%s"
unicode(7) "0.0.0.0"
Done
