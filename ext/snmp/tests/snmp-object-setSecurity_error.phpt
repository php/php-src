--TEST--
OO API: SNMP::setSecurity (errors)
--CREDITS--
Boris Lytochkin
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

$session = new SNMP(SNMP::VERSION_3, $hostname, $user_noauth, $timeout, $retries);
$session->setSecurity('noAuthNoPriv');

#echo "Checking error handling\n";

try {
var_dump($session->setSecurity(''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
var_dump($session->setSecurity('bugusPriv'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
var_dump($session->setSecurity('authNoPriv', 'TTT'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump($session->setSecurity('authNoPriv', 'MD5', ''));
var_dump($session->setSecurity('authNoPriv', 'MD5', 'te'));

try {
    var_dump(snmp3_get($hostname, $community, 'authPriv', 'MD5', $auth_pass, 'BBB', '', '.1.3.6.1.2.1.1.1.0'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'BBB'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', ''));
var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', 'ty'));
var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', 'test12345', 'context', 'dsa'));

var_dump($session->close());

?>
--EXPECTF--
Security level must be one of "noAuthNoPriv", "authNoPriv", or "authPriv"
Security level must be one of "noAuthNoPriv", "authNoPriv", or "authPriv"
Authentication protocol must be %s

Warning: SNMP::setSecurity(): Error generating a key for authentication pass phrase '': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Error generating a key for authentication pass phrase 'te': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)
Security protocol must be one of "DES", "AES128", or "AES"
Security protocol must be one of "DES", "AES128", or "AES"

Warning: SNMP::setSecurity(): Error generating a key for privacy pass phrase '': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Error generating a key for privacy pass phrase 'ty': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Bad engine ID value 'dsa' in %s on line %d
bool(false)
bool(true)
