--TEST--                                 
OO API: getErrno & getError methods
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
snmp_set_enum_print(false);
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
snmp_set_oid_output_format(SNMP_OID_OUTPUT_FULL);

echo "SNMP::ERRNO_NOERROR\n";
$session = new SNMP(SNMP_VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump(@$session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getErrno() == SNMP::ERRNO_NOERROR);
var_dump($session->getError());
$session->close();

echo "SNMP::ERRNO_TIMEOUT\n";
$session = new SNMP(SNMP_VERSION_2c, $hostname, 'timeout_community_432', $timeout, $retries);
$session->valueretrieval = SNMP_VALUE_LIBRARY;
var_dump(@$session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getErrno() == SNMP::ERRNO_TIMEOUT);
var_dump($session->getError());
$session->close();
echo "SNMP::ERRNO_ERROR_IN_REPLY\n";
$session = new SNMP(SNMP_VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump(@$session->get('.1.3.6.1.2.1.1.1.110'));
var_dump($session->getErrno() == SNMP::ERRNO_ERROR_IN_REPLY);
var_dump($session->getError());
$session->close();
echo "SNMP::ERRNO_GENERIC\n";
$session = new SNMP(SNMP_VERSION_3, $hostname, 'somebogususer', $timeout, $retries);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass);
var_dump(@$session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->getErrno() == SNMP::ERRNO_GENERIC);
var_dump($session->getError());
$session->close();
?>
--EXPECTF--
SNMP::ERRNO_NOERROR
%string|unicode%(%d) "%s"
bool(true)
%string|unicode%(0) ""
SNMP::ERRNO_TIMEOUT
bool(false)
bool(true)
%string|unicode%(%d) "No response from %s"
SNMP::ERRNO_ERROR_IN_REPLY
bool(false)
bool(true)
%string|unicode%(%d) "Error in packet %s"
SNMP::ERRNO_GENERIC
bool(false)
bool(true)
%string|unicode%(%d) "Fatal error: Unknown user name"
