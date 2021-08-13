--TEST--
reusing persistent connections
--EXTENSIONS--
oci8
--FILE--
<?php

require __DIR__."/connect.inc";

var_dump(oci_pconnect($user, $password, $dbase));
var_dump(oci_pconnect($user, $password, $dbase));
var_dump(oci_pconnect($user, $password, $dbase));
var_dump(oci_connect($user, $password, $dbase));
var_dump(oci_connect($user, $password, $dbase));
var_dump(oci_connect($user, $password, $dbase));

echo "Done\n";
?>
--EXPECTF--
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
Done
