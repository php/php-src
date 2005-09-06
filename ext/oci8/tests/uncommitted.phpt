--TEST--
uncommitted connection
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

$stmt = oci_parse($c, "select 1 from dual");
oci_execute($stmt, OCI_DEFAULT);

echo "Done\n";
?>
--EXPECTF--	
Done
