--TEST--
SNMPv3 Support
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

//int snmp3_get(string host, string sec_name, string sec_level, string auth_protocol,
//              string auth_passphrase, string priv_protocol, string priv_passphrase,
//              string object_id [, int timeout [, int retries]]);

echo "Working version\n";
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
echo "GET single: noAuthNoPriv\n";
var_dump(snmp3_get($hostname, $user_noauth, 'noAuthNoPriv', '', '', '', '', '.1.3.6.1.2.1.1.1.0', $timeout, $retries));

foreach(array('MD5', 'SHA') as $signalg) {
	echo "GET single: $signalg\n";
	var_dump(snmp3_get($hostname, $user_auth_prefix . $signalg, 'authNoPriv', $signalg, $auth_pass, '', '', '.1.3.6.1.2.1.1.1.0', $timeout, $retries));
}

foreach(array('AES', 'DES', 'AES128') as $chipher) {
	echo "GET single: MD5/$chipher\n";
	var_dump(snmp3_get($hostname, $user_auth_prefix . 'MD5' . $chipher, 'authPriv', 'MD5', $auth_pass, $chipher, $priv_pass, '.1.3.6.1.2.1.1.1.0', $timeout, $retries));
}
$username = $user_auth_prefix . 'MD5';
echo "GET multiple\n";
var_dump(snmp3_get($hostname, $username, 'authNoPriv', 'MD5', $auth_pass, '', '', array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.3.0'), $timeout, $retries));
echo "GETNEXT single\n";
var_dump(snmp3_getnext($hostname, $username, 'authNoPriv', 'MD5', $auth_pass, '', '', '.1.3.6.1.2.1.1.1.0', $timeout, $retries));

echo "WALK single on single OID\n";
$z = snmp3_walk($hostname, $username, 'authNoPriv', 'MD5', $auth_pass, '', '', '.1.3.6.1.2.1.1.1.0', $timeout, $retries);
var_dump(gettype($z));
var_dump(count($z));
var_dump($z);

echo "REALWALK single on single OID\n";
$z = snmp3_real_walk($hostname, $username, 'authNoPriv', 'MD5', $auth_pass, '', '', '.1.3.6.1.2.1.1.1.0', $timeout, $retries);
var_dump(gettype($z));
var_dump(count($z));
var_dump($z);

echo "WALK multiple on single OID\n";
$z = snmp3_walk($hostname, $username, 'authNoPriv', 'MD5', $auth_pass, '', '', '.1.3.6.1.2.1.1', $timeout, $retries);
var_dump(gettype($z));
var_dump(count($z));
var_dump(key($z));
var_dump(array_shift($z));

echo "REALWALK multiple on single OID\n";
$z = snmp3_real_walk($hostname, $username, 'authNoPriv', 'MD5', $auth_pass, 'AES', '', '.1.3.6.1.2.1.1', $timeout, $retries);
var_dump(gettype($z));
var_dump(count($z));
var_dump(key($z));
var_dump(array_shift($z));

echo "SET single OID\n";
$oid1 = 'SNMPv2-MIB::sysContact.0';
$oldvalue1 = snmp3_get($hostname, $rwuser, 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, $oid1, $timeout, $retries);
$newvalue1 = $oldvalue1 . '0';

$z = snmp3_set($hostname, $rwuser, 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, $oid1, 's', $newvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmp3_get($hostname, $rwuser, 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, $oid1, $timeout, $retries) === $newvalue1));
$z = snmp3_set($hostname, $rwuser, 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, $oid1, 's', $oldvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmp3_get($hostname, $rwuser, 'authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, $oid1, $timeout, $retries) === $oldvalue1));

?>
--EXPECTF--
Working version
GET single: noAuthNoPriv
string(%d) "%s"
GET single: MD5
string(%d) "%s"
GET single: SHA
string(%d) "%s"
GET single: MD5/AES
string(%d) "%s"
GET single: MD5/DES
string(%d) "%s"
GET single: MD5/AES128
string(%d) "%s"
GET multiple
array(2) {
  ["%s"]=>
  string(%d) "%s"
  ["%s"]=>
  string(%d) "%s"
}
GETNEXT single
string(%d) "%s"
WALK single on single OID
string(5) "array"
int(1)
array(1) {
  [0]=>
  string(%d) "%s"
}
REALWALK single on single OID
string(5) "array"
int(1)
array(1) {
  ["%s"]=>
  string(%d) "%s"
}
WALK multiple on single OID
string(5) "array"
int(%d)
int(0)
string(%d) "%s"
REALWALK multiple on single OID
string(5) "array"
int(%d)
string(%d) "%s"
string(%d) "%s"
SET single OID
bool(true)
bool(true)
bool(true)
bool(true)
