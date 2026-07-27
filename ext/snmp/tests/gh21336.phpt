--TEST--
GH-21336 (undefined behavior in snmp - NULL pointer dereference in setSecurity)
--EXTENSIONS--
snmp
--FILE--
<?php
$session = new SNMP(SNMP::VERSION_3, 'localhost', 'user');

// auth protocol NULL
try {
	$session->setSecurity('authPriv');
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

// auth passphrase NULL
try {
	$session->setSecurity('authNoPriv', 'MD5');
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

// priv protocol NULL
try {
	$session->setSecurity('authPriv', 'MD5', 'test12345');
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

// priv passphrase NULL
try {
	$session->setSecurity('authPriv', 'MD5', 'test12345', 'AES');
} catch (ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECT--
SNMP::setSecurity(): Argument #2 ($authProtocol) cannot be null when security level is "authNoPriv" or "authPriv"
SNMP::setSecurity(): Argument #3 ($authPassphrase) cannot be null when security level is "authNoPriv" or "authPriv"
SNMP::setSecurity(): Argument #4 ($privacyProtocol) cannot be null when security level is "authPriv"
SNMP::setSecurity(): Argument #5 ($privacyPassphrase) cannot be null when security level is "authPriv"
