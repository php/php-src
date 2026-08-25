--TEST--
GH-23453 (ext/snmp: Attempted free on non-malloced address)
--EXTENSIONS--
snmp
--FILE--
<?php
$session = new SNMP(SNMP::VERSION_3, 'localhost', 'user');

// 32 bytes is the maximum length of a context engine ID
var_dump($session->setSecurity('authPriv', 'SHA', 'authpassword12345', 'AES', 'privpassword12345', 'myContext', str_repeat('aa', 32)));
var_dump($session->setSecurity('authPriv', 'SHA', 'authpassword12345', 'AES', 'privpassword12345', 'myContext', str_repeat('aa', 33)));
?>
--EXPECTF--
bool(true)

Warning: SNMP::setSecurity(): Bad engine ID value 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa' in %s on line %d
bool(false)
