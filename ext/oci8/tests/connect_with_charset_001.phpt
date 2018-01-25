--TEST--
oci_connect() with invalid character set
--SKIPIF--
<?php if (!extension_loaded("oci8")) print "skip"; ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

var_dump($c1 = oci_connect($user, $password, $dbase));
var_dump($c2 = oci_connect($user, $password, $dbase, ""));
var_dump($c3 = oci_connect($user, $password, $dbase, "blah"));
var_dump($c4 = oci_connect($user, $password, $dbase, "obviously wrong"));

var_dump($c3 == $c4);

var_dump($c5 = oci_connect($user, $password, $dbase, "US7ASCII"));
var_dump($c6 = oci_connect($user, $password, $dbase, "UTF8"));

var_dump($c5 == $c6);

echo "Done\n";
?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)

Warning: oci_connect(): Invalid character set name: blah in %s on line %d
resource(%d) of type (oci8 connection)

Warning: oci_connect(): Invalid character set name: obviously wrong in %s on line %d
resource(%d) of type (oci8 connection)
bool(true)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
bool(false)
Done
