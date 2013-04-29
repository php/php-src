--TEST--                                 
OO API: SNMP::setSecurity (errors)
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

$session = new SNMP(SNMP::VERSION_3, $hostname, $user_noauth, $timeout, $retries);
$session->setSecurity('noAuthNoPriv');

#echo "Checking error handling\n";
var_dump($session->setSecurity());
var_dump($session->setSecurity(''));
var_dump($session->setSecurity('bugusPriv'));
var_dump($session->setSecurity('authNoPriv', 'TTT'));
var_dump($session->setSecurity('authNoPriv', 'MD5', ''));
var_dump($session->setSecurity('authNoPriv', 'MD5', 'te'));
var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'BBB'));
var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', ''));
var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', 'ty'));
var_dump($session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', 'test12345', 'context', 'dsa'));

var_dump($session->close());

?>
--EXPECTF--

Warning: SNMP::setSecurity() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Invalid security level '' in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Invalid security level 'bugusPriv' in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Unknown authentication protocol 'TTT' in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Error generating a key for authentication pass phrase '': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Error generating a key for authentication pass phrase 'te': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Unknown security protocol 'BBB' in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Error generating a key for privacy pass phrase '': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Error generating a key for privacy pass phrase 'ty': Generic error (The supplied password length is too short.) in %s on line %d
bool(false)

Warning: SNMP::setSecurity(): Bad engine ID value 'dsa' in %s on line %d
bool(false)
bool(true)
