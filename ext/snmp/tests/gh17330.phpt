--TEST--
SNMP::setSecurity() segfault when the object had been closed.
--EXTENSIONS--
snmp
--CREDITS--
YuanchengJiang 
--FILE--
<?php
$session = new SNMP(SNMP::VERSION_2c, "localhost", 'timeout_community_432');
$session->close();
try {
	$session->setSecurity('authPriv', 'MD5', '', 'AES', '');
} catch(Error $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
Invalid or uninitialized SNMP object
