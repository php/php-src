--TEST--
SNMP::setSecurity() - the context name is copied
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

$session = new SNMP(SNMP::VERSION_3, $hostname, $rwuser, $timeout, $retries);
$context = str_repeat('c', 5) . random_int(10, 99);
$session->setSecurity('authPriv', 'MD5', $auth_pass, 'AES', $priv_pass, $context);
unset($context);
// The agent does not know this context, only the lifetime of the name matters.
var_dump(@$session->get('.1.3.6.1.2.1.1.1.0'));
$session->close();
?>
--EXPECT--
bool(false)
