--TEST--
oci_collection_assign()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--ENV--
return "
ORACLE_HOME=".(isset($_ENV['ORACLE_HOME']) ? $_ENV['ORACLE_HOME'] : '')."
NLS_LANG=".(isset($_ENV['NLS_LANG']) ? $_ENV['NLS_LANG'] : '')."
TNS_ADMIN=".(isset($_ENV['TNS_ADMIN']) ? $_ENV['TNS_ADMIN'] : '')."
";
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_type.inc";

$coll1 = oci_new_collection($c, $type_name);
$coll2 = oci_new_collection($c, $type_name);

var_dump(oci_collection_append($coll1, 1));

var_dump(oci_collection_assign($coll2, $coll1));

var_dump(oci_collection_element_get($coll2, 0));

echo "Done\n";

require dirname(__FILE__)."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
float(1)
Done
