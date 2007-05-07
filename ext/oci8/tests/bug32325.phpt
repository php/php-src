--TEST--
Bug #32325 (Can't retrieve collection using OCI8)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';
			
$create_stmt = oci_parse($c, "create or replace type ut_num_list_t as table of number");
oci_execute($create_stmt);

$collection = oci_new_collection($c, "UT_NUM_LIST_T");

$sql  = "
        begin
		select ut_num_list_t(1,2,3,4) into :list from dual;
		end;";

$stmt = oci_parse($c, $sql);

oci_bind_by_name($stmt, ":list",  $collection, -1, OCI_B_NTY);
oci_execute($stmt);

var_dump($collection->size());
var_dump($collection->getelem(1));
var_dump($collection->getelem(2));

$drop_stmt = oci_parse($c, "drop type ut_num_list_t");
oci_execute($drop_stmt);

echo "Done\n";
?>
--EXPECTF--	
int(4)
float(2)
float(3)
Done
