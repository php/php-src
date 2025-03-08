--TEST--
OO API: getErrno & getError methods
--CREDITS--
Boris Lytochkin
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (getenv('SKIP_ASAN')) die('skip Timeouts under ASAN');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_enum_print(false);
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
snmp_set_oid_output_format(SNMP_OID_OUTPUT_FULL);

echo "SNMP::ERRNO_NOERROR\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump(@$session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getErrno() == SNMP::ERRNO_NOERROR);
var_dump($session->getError());
$session->close();

echo "SNMP::ERRNO_TIMEOUT\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, 'timeout_community_432', $timeout, $retries);
$session->valueretrieval = SNMP_VALUE_LIBRARY;
var_dump(@$session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getErrno() == SNMP::ERRNO_TIMEOUT);
var_dump($session->getError());
$session->close();

echo "SNMP::ERRNO_ERROR_IN_REPLY\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump(@$session->get('.1.3.6.1.2.1.1.1.110'));
var_dump($session->getErrno() == SNMP::ERRNO_ERROR_IN_REPLY);
var_dump($session->getError());
$session->close();

echo "SNMP::ERRNO_GENERIC\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, 'somebogususer', $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass);
var_dump(@$session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getErrno() == SNMP::ERRNO_GENERIC);
var_dump($session->getError());
var_dump(@$session->get(array('.1.3.6.1.2.1.1.1.0')));
$session->close();

echo "SNMP::ERRNO_OID_PARSING_ERROR\n";
echo "GET: Single wrong OID\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump(@$session->get('.1.3.6.1.2..1.1.1.0'));
var_dump($session->getErrno() == SNMP::ERRNO_OID_PARSING_ERROR);
var_dump($session->getError());
$session->close();
echo "GET: Miltiple OID, one wrong\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump(@$session->get(array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2..1.1.1.0')));
var_dump($session->getErrno() == SNMP::ERRNO_OID_PARSING_ERROR);
var_dump($session->getError());
$session->close();
echo "WALK: Single wrong OID\n";
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump(@$session->walk('.1.3.6.1.2..1.1'));
var_dump($session->getErrno() == SNMP::ERRNO_OID_PARSING_ERROR);
var_dump($session->getError());
$session->close();
echo "SET: Wrong type\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $rwuser, $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass);
$oid1 = 'SNMPv2-MIB::sysContact.0';
var_dump(@$session->set($oid1, 'q', 'blah'));
var_dump($session->getErrno() == SNMP::ERRNO_OID_PARSING_ERROR);
var_dump($session->getError());

echo "SNMP::ERRNO_MULTIPLE_SET_QUERIES\n";
$oid1 = 'SNMPv2-MIB::sysContact.0';
$oid2 = 'SNMPv2-MIB::sysLocation.0';
$session = new SNMP(SNMP::VERSION_3, $hostname, $rwuser, $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass);
$session->max_oids = 1;
$oldvalue1 = $session->get($oid1);
$newvalue1 = $oldvalue1 . '0';
$oldvalue2 = $session->get($oid2);
$newvalue2 = $oldvalue2 . '0';
$z = @$session->set(array($oid1, $oid2), array('s','s'), array($newvalue1, $newvalue2));
var_dump($z);
var_dump($session->getErrno() == SNMP::ERRNO_MULTIPLE_SET_QUERIES);
var_dump($session->getError());
var_dump(($session->get($oid1) === $newvalue1));
var_dump(($session->get($oid2) === $newvalue2));
$z = @$session->set(array($oid1, $oid2), array('s','s'), array($oldvalue1, $oldvalue2));
var_dump($z);
var_dump($session->getErrno() == SNMP::ERRNO_MULTIPLE_SET_QUERIES);
var_dump($session->getError());
var_dump(($session->get($oid1) === $oldvalue1));
var_dump(($session->get($oid2) === $oldvalue2));
var_dump($session->close());
?>
--EXPECTF--
SNMP::ERRNO_NOERROR
string(%d) "%s"
bool(true)
string(0) ""
SNMP::ERRNO_TIMEOUT
bool(false)
bool(true)
string(%d) "No response from %s"
SNMP::ERRNO_ERROR_IN_REPLY
bool(false)
bool(true)
string(%d) "Error in packet %s"
SNMP::ERRNO_GENERIC
bool(false)
bool(true)
string(%d) "Fatal error: Unknown user name"
bool(false)
SNMP::ERRNO_OID_PARSING_ERROR
GET: Single wrong OID
bool(false)
bool(true)
string(46) "Invalid object identifier: .1.3.6.1.2..1.1.1.0"
GET: Miltiple OID, one wrong
bool(false)
bool(true)
string(46) "Invalid object identifier: .1.3.6.1.2..1.1.1.0"
WALK: Single wrong OID
bool(false)
bool(true)
string(42) "Invalid object identifier: .1.3.6.1.2..1.1"
SET: Wrong type
bool(false)
bool(true)
string(129) "Could not add variable: OID='.iso.org.dod.internet.mgmt.mib-2.system.sysContact.0' type='q' value='blah': Bad variable type ("q")"
SNMP::ERRNO_MULTIPLE_SET_QUERIES
bool(true)
bool(true)
string(73) "Cannot fit all OIDs for SET query into one packet, using multiple queries"
bool(true)
bool(true)
bool(true)
bool(true)
string(73) "Cannot fit all OIDs for SET query into one packet, using multiple queries"
bool(true)
bool(true)
bool(true)
