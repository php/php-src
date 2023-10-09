--TEST--
Unsupported type: oci_bind_array_by_name() and SQLT_BFLOAT
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
if (!defined('SQLT_BFLOAT')) die('skip SQLT_BFLOAT type not available on older Oracle clients');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$statement = oci_parse($c, "BEGIN array_bind_bfloat_pkg.iobind(:c1); END;");
$array = Array(1.243,2.5658,3.4234,4.2123,5.9999);
oci_bind_array_by_name($statement, ":c1", $array, 5, 5, SQLT_BFLOAT);

echo "Done\n";
?>
--EXPECTF--
Warning: oci_bind_array_by_name(): Unknown or unsupported datatype given: 21 in %sarray_bind_bfloat.php on line %d
Done
