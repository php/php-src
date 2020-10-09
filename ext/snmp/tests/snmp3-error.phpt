--TEST--
SNMPv3 Support (errors)
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking error handling\n";

//int snmp3_get(string host, string sec_name, string sec_level, string auth_protocol,
//              string auth_passphrase, string priv_protocol, string priv_passphrase,
//              string object_id [, int timeout [, int retries]]);

try {
    var_dump(snmp3_get($hostname, $community, '', '', '', '', '', '.1.3.6.1.2.1.1.1.0'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(snmp3_get($hostname, $community, 'bugusPriv', '', '', '', '', '.1.3.6.1.2.1.1.1.0'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(snmp3_get($hostname, $community, 'authNoPriv', 'TTT', '', '', '', '.1.3.6.1.2.1.1.1.0'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(snmp3_get($hostname, $community, 'authNoPriv', 'MD5', '', '', '', '.1.3.6.1.2.1.1.1.0'));
var_dump(snmp3_get($hostname, $community, 'authNoPriv', 'MD5', 'te', '', '', '.1.3.6.1.2.1.1.1.0'));

try {
    var_dump(snmp3_get($hostname, $community, 'authPriv', 'MD5', $auth_pass, 'BBB', '', '.1.3.6.1.2.1.1.1.0'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(snmp3_get($hostname, $community, 'authPriv', 'MD5', $auth_pass, 'AES', '', '.1.3.6.1.2.1.1.1.0'));
var_dump(snmp3_get($hostname, $community, 'authPriv', 'MD5', $auth_pass, 'AES', 'ty', '.1.3.6.1.2.1.1.1.0'));
var_dump(snmp3_get($hostname, 'somebogususer', 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, '.1.3.6.1.2.1.1.1.0', $timeout, $retries));

var_dump(snmp3_set($hostname, $rwuser, 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, '.1.3.6.777...7.5.3', 's', 'ttt', $timeout, $retries));

try {
    var_dump(snmp3_set($hostname, $rwuser, 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, '.1.3.6.777.7.5.3', array('s'), 'yyy', $timeout, $retries));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
Checking error handling
Security level must be one of "noAuthNoPriv", "authNoPriv", or "authPriv"
Security level must be one of "noAuthNoPriv", "authNoPriv", or "authPriv"
Authentication protocol must be either "MD5" or "SHA"

Warning: snmp3_get(): Error generating a key for authentication pass phrase '': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: snmp3_get(): Error generating a key for authentication pass phrase 'te': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)
Security protocol must be one of "DES", "AES128", or "AES"

Warning: snmp3_get(): Error generating a key for privacy pass phrase '': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: snmp3_get(): Error generating a key for privacy pass phrase 'ty': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: snmp3_get(): Fatal error: Unknown user name in %s on line %d
bool(false)

Warning: snmp3_set(): Invalid object identifier: .1.3.6.777...7.5.3 in %s on line %d
bool(false)
Type must be of type string when object ID is a string
