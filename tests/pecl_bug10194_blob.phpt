--TEST--
PECL Bug #10194 (segfault in Instant Client when memory_limit is reached inside the callback) 
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
?>
--INI--
memory_limit=3M
--FILE--
<?php

// This test is dependent on the behavior of the memory manager
	
require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

$ora_sql = "INSERT INTO ".$schema.$table_name." (blob)
                          VALUES (empty_blob())";

$statement = oci_parse($c,$ora_sql);
oci_execute($statement);

$ora_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$statement = oci_parse($c,$ora_sql);
oci_execute($statement, OCI_DEFAULT);

$row = oci_fetch_assoc($statement);

$string = str_repeat("test", 32768*4*4);

for ($i = 0; $i < 8; $i++) {
	$row['BLOB']->write($string);
}

oci_commit($c);

$ora_sql = "SELECT blob FROM ".$schema.$table_name;
$statement = oci_parse($c,$ora_sql);
oci_execute($statement);

echo "Before load()\n";

$row = oci_fetch_assoc($statement);
var_dump(strlen($row['BLOB']->load())); /* here it should fail */

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";
?>
--EXPECTF--	
Before load()

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
