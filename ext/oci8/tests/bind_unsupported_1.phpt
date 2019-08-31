--TEST--
Bind with various unsupported bind types
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// These types are defined in oci8.c

$types = array(
    "SQLT_AVC" => SQLT_AVC,
    "SQLT_STR" => SQLT_STR,
    "SQLT_VCS" => SQLT_VCS,
    "SQLT_AVC" => SQLT_AVC,
    "SQLT_STR" => SQLT_STR,
    "SQLT_LVC" => SQLT_LVC,
    "SQLT_FLT" => SQLT_FLT,
    "SQLT_UIN" => SQLT_UIN,
    "SQLT_ODT" => SQLT_ODT,
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
Test - SQLT_AVC

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 97 in %sbind_unsupported_1.php on line %d
Test - SQLT_STR

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 5 in %sbind_unsupported_1.php on line %d
Test - SQLT_VCS

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 9 in %sbind_unsupported_1.php on line %d
Test - SQLT_LVC

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 94 in %sbind_unsupported_1.php on line %d
Test - SQLT_FLT

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 4 in %sbind_unsupported_1.php on line %d
Test - SQLT_UIN

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 68 in %sbind_unsupported_1.php on line %d
Test - SQLT_ODT

Warning: oci_bind_by_name(): Unknown or unsupported datatype given: 156 in %sbind_unsupported_1.php on line %d
===DONE===
