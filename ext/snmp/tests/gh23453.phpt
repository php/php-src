--TEST--
GH-23453 (SNMP::setSecurity() frees a non-malloced address with a context engine ID longer than 32 bytes)
--EXTENSIONS--
snmp
--FILE--
<?php
$session = new SNMP(SNMP::VERSION_3, 'localhost', 'user');

// 32 bytes is the maximum length of a context engine ID
var_dump($session->setSecurity('authPriv', 'SHA', 'authpassword12345', 'AES', 'privpassword12345', 'myContext', str_repeat('aa', 32)));
try {
    var_dump($session->setSecurity('authPriv', 'SHA', 'authpassword12345', 'AES', 'privpassword12345', 'myContext', str_repeat('aa', 33)));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
bool(true)
ValueError: SNMP::setSecurity(): Argument #7 ($contextEngineId) must be a valid context engine ID
