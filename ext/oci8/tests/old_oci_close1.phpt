--TEST--
oci8.old_oci_close_semantics Off
--EXTENSIONS--
oci8
--INI--
oci8.old_oci_close_semantics=0
--FILE--
<?php

require __DIR__."/connect.inc";

var_dump($c);
var_dump(oci_close($c));

try {
    var_dump(oci_parse($c, "select 1 from dual"));
} catch(\TypeError $exception) {
    var_dump($exception->getMessage());
}

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
bool(true)
string(%d) "oci_parse(): supplied resource is not a valid oci8 connection resource"
Done
