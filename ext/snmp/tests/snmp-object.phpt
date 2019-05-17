--TEST--
OO API
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_enum_print(false);
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
snmp_set_oid_output_format(SNMP_OID_OUTPUT_FULL);

echo "SNMPv1\n";
$session = new SNMP(SNMP::VERSION_1, $hostname, $community, $timeout, $retries);
$session->valueretrieval = SNMP_VALUE_LIBRARY;
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->get(array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.3.0')));
var_dump($session->getnext('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

echo "SNMPv2\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getnext('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

echo "GET with preserving original OID names\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
$orig = array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.5.0');
$result = $session->get($orig, TRUE);
foreach($orig as $oid){
	var_dump($result[$oid]);
}
var_dump($session->close());

echo "WALK multiple on single OID\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
$z = $session->walk('.1.3.6.1.2.1.1');
var_dump(gettype($z));
var_dump(count($z));
var_dump(key($z));
var_dump(array_shift($z));
var_dump($session->close());

echo "WALK multiple on single OID, max_repetitions set to 30\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
$z = $session->walk('.1.3.6.1.2.1.1', FALSE, 30);
var_dump(gettype($z));
var_dump(count($z));
var_dump(key($z));
var_dump(array_shift($z));
var_dump($session->close());

echo "WALK multiple on single OID, max_repetitions set to 30, non_repeaters set to 0\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
$z = $session->walk('.1.3.6.1.2.1.1', FALSE, 30, 0);
var_dump(gettype($z));
var_dump(count($z));
var_dump(key($z));
var_dump(array_shift($z));
var_dump($session->close());

echo "WALK multiple on single OID, max_oids set to 30\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
$session->max_oids = 30;
$z = $session->walk('.1.3.6.1.2.1.1');
var_dump(gettype($z));
var_dump(count($z));
var_dump(key($z));
var_dump(array_shift($z));
var_dump($session->close());

echo "WALK multiple on single OID with OID suffix as keys\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
$z = $session->walk('.1.3.6.1.2.1.1', TRUE);
var_dump(gettype($z));
var_dump(count($z));
var_dump(key($z));
var_dump(array_shift($z));
var_dump(key($z));
array_shift($z);
var_dump(key($z));
array_shift($z);
var_dump(key($z));
array_shift($z);
var_dump(key($z));
var_dump($session->close());

echo "SNMPv3 (default security settings)\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $user_noauth, $timeout, $retries);
#$session->setSecurity($user_noauth, 'noAuthNoPriv', '', '', '', '', '', '');
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getnext('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

echo "SNMPv3 (noAuthNoPriv)\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $user_noauth, $timeout, $retries);
$session->setSecurity('noAuthNoPriv');
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getnext('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

echo "SNMPv3 (authPriv)\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $rwuser, $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass);
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getnext('.1.3.6.1.2.1.1.1.0'));
var_dump($session->walk('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

echo "SET single OID\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $rwuser, $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass);
$oid1 = 'SNMPv2-MIB::sysContact.0';
$oldvalue1 = $session->get($oid1);
$newvalue1 = $oldvalue1 . '0';

$z = $session->set($oid1, 's', $newvalue1);
var_dump($z);
var_dump(($session->get($oid1) === $newvalue1));
$z = $session->set($oid1, 's', $oldvalue1);
var_dump($z);
var_dump(($session->get($oid1) === $oldvalue1));
var_dump($session->close());

echo "Multiple OID with max_oids = 1\n";
$oid2 = 'SNMPv2-MIB::sysLocation.0';
$session = new SNMP(SNMP::VERSION_3, $hostname, $rwuser, $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass);
$session->max_oids = 1;
$oldvalue2 = $session->get($oid2);
$newvalue2 = $oldvalue2 . '0';
$z = $session->set(array($oid1, $oid2), array('s','s'), array($newvalue1, $newvalue2));
var_dump($z);
var_dump(($session->get($oid1) === $newvalue1));
var_dump(($session->get($oid2) === $newvalue2));
$z = $session->set(array($oid1, $oid2), array('s','s'), array($oldvalue1, $oldvalue2));
var_dump($z);
var_dump(($session->get($oid1) === $oldvalue1));
var_dump(($session->get($oid2) === $oldvalue2));
var_dump($session->close());

echo "SNMPv3, setting contextEngineID (authPriv)\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $rwuser, $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, '', 'aeeeff');
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

?>
--EXPECTF--
SNMPv1
string(%d) "%S"
array(2) {
  ["%s"]=>
  string(%d) "%S"
  ["%s"]=>
  string(%d) "%S"
}
string(%d) "%S"
bool(true)
SNMPv2
string(%d) "%S"
string(%d) "%S"
bool(true)
GET with preserving original OID names
string(%d) "%s"
string(%d) "%s"
bool(true)
WALK multiple on single OID
string(5) "array"
int(%d)
string(%d) "%S"
string(%d) "%S"
bool(true)
WALK multiple on single OID, max_repetitions set to 30
string(5) "array"
int(%d)
string(%d) "%S"
string(%d) "%S"
bool(true)
WALK multiple on single OID, max_repetitions set to 30, non_repeaters set to 0
string(5) "array"
int(%d)
string(%d) "%S"
string(%d) "%S"
bool(true)
WALK multiple on single OID, max_oids set to 30
string(5) "array"
int(%d)
string(%d) "%S"
string(%d) "%S"
bool(true)
WALK multiple on single OID with OID suffix as keys
string(5) "array"
int(%d)
string(3) "1.0"
string(%d) "%s"
string(3) "2.0"
string(3) "3.0"
string(3) "4.0"
string(3) "5.0"
bool(true)
SNMPv3 (default security settings)
string(%d) "%S"
string(%d) "%S"
bool(true)
SNMPv3 (noAuthNoPriv)
string(%d) "%S"
string(%d) "%S"
bool(true)
SNMPv3 (authPriv)
string(%d) "%S"
string(%d) "%S"
array(1) {
  [%s]=>
  string(%d) "%S"
}
bool(true)
SET single OID
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Multiple OID with max_oids = 1

Warning: SNMP::set(): Can not fit all OIDs for SET query into one packet, using multiple queries in %s on line %d
bool(true)
bool(true)
bool(true)

Warning: SNMP::set(): Can not fit all OIDs for SET query into one packet, using multiple queries in %s on line %d
bool(true)
bool(true)
bool(true)
bool(true)
SNMPv3, setting contextEngineID (authPriv)
string(%d) "%S"
bool(true)
