--TEST--
DRCP: oci_pconnect() and oci_connect() with different character sets
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__."/details.inc";

// Create connections with oci_connect and oci_pconnect with UTF8 as Charset

$c1 = oci_connect($user,$password,$dbase,"UTF8");
var_dump($c1);

// Now with oci_pconnect()

$p1 = oci_pconnect($user,$password,$dbase,"UTF8");
var_dump($p1);

// Create two more connections with character set US7ASCII

$c2 = oci_connect($user,$password,$dbase,"US7ASCII");
var_dump($c2);

// Now with oci_pconnect()

$p2 = oci_pconnect($user,$password,$dbase,"US7ASCII");
var_dump($p2);

// The two connections c1 and c2 should not share resources as they use different
//character sets

if((int)$c1 === (int)$c2)
    echo "First and third connections share a resource: NOT OK\n";
else
    echo "First and third  connections are different: OK\n";

// The two connections p1 and p2 should not share resources as they use different
//character sets

if((int)$p1 === (int)$p2)
    echo "Second and fourth connections share a resource: NOT OK\n";
else
    echo "Second and fourth connections are different: OK\n";

// Close all the connections
oci_close($c1);
oci_close($c2);
oci_close($p1);
oci_close($p2);

echo "Done\n";
?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 persistent connection)
First and third  connections are different: OK
Second and fourth connections are different: OK
Done
