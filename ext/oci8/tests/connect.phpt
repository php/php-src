--TEST--
oci_connect()
--EXTENSIONS--
oci8
--FILE--
<?php

require __DIR__."/connect.inc";

if (!empty($dbase)) {
    var_dump(oci_connect($user, $password, $dbase));
}
else {
    var_dump(oci_connect($user, $password));
}

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
Done
