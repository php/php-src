--TEST--
Bind with various unsupported 10g+ bind types
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$types = array(
    "SQLT_BDOUBLE" => SQLT_BDOUBLE,
    "SQLT_BFLOAT" => SQLT_BFLOAT,
);

foreach ($types as $t => $v) {

    echo "Test - $t\n";

    $s = oci_parse($c, "select * from dual where dummy = :c1");
    $c1 = "Doug";
    oci_bind_by_name($s, ":c1", $c1, -1, $v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test - SQLT_BDOUBLE

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 22 in %sbind_unsupported_2.php on line %d
Test - SQLT_BFLOAT

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 21 in %sbind_unsupported_2.php on line %d
===DONE===
