--TEST--
connect/close/connect
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--ENV--
return "
ORACLE_HOME=".(isset($_ENV['ORACLE_HOME']) ? $_ENV['ORACLE_HOME'] : '')."
NLS_LANG=".(isset($_ENV['NLS_LANG']) ? $_ENV['NLS_LANG'] : '')."
";
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

oci_close($c);

oci_connect($user, $password, $dbase);
	
echo "Done\n";
?>
--EXPECTF--	
Done
