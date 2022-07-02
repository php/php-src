--TEST--
oci_pconnect() & oci_new_connect()
--EXTENSIONS--
oci8
--FILE--
<?php

require __DIR__."/connect.inc";

if (!empty($dbase)) {
    var_dump($c1 = oci_pconnect($user, $password, $dbase));
}
else {
    var_dump($c1 = oci_pconnect($user, $password));
}

if (!empty($dbase)) {
    var_dump($c2 = oci_new_connect($user, $password, $dbase));
}
else {
    var_dump($c2 = oci_new_connect($user, $password));
}

var_dump(oci_close($c1));
var_dump(oci_close($c2));

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 connection)
bool(true)
bool(true)
Done
