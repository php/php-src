--TEST--
OO API: Generic errors
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

try {
	var_dump(new SNMP(SNMP::VERSION_1, $hostname));
} catch (TypeError $e) {
	print $e->getMessage() . "\n";
}
try {
	var_dump(new SNMP(SNMP::VERSION_1, $hostname, $community, ''));
} catch (TypeError $e) {
	print $e->getMessage() . "\n";
}
try {
	var_dump(new SNMP(SNMP::VERSION_1, $hostname, $community, $timeout, ''));
} catch (TypeError $e) {
	print $e->getMessage() . "\n";
}
try {
	var_dump(new SNMP(7, $hostname, $community));
} catch (Exception $e) {
	print $e->getMessage() . "\n";
}

echo "Exception handling\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $user_noauth, $timeout, $retries);
try {
	var_dump($session->get('.1.3.6.1.2.1.1.1..0'));
} catch (SNMPException $e) {
	var_dump($e->getCode());
	var_dump($e->getMessage());
}
$session->exceptions_enabled = SNMP::ERRNO_ANY;
try {
	var_dump($session->get('.1.3.6.1.2.1.1.1..0'));
} catch (SNMPException $e) {
	var_dump($e->getCode());
	var_dump($e->getMessage());
}
var_dump($session->close());

echo "Open normal session\n";
$session = new SNMP(SNMP::VERSION_3, $hostname, $user_noauth, $timeout, $retries);
$session->valueretrieval = 67;
var_dump($session->valueretrieval);
echo "Closing session\n";
var_dump($session->close(''));
var_dump($session->close());
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
var_dump($session->walk('.1.3.6.1.2.1.1', FALSE, ''));
var_dump($session->walk('.1.3.6.1.2.1.1', FALSE, 30, ''));
var_dump($session->get());
var_dump($session->getnext());
var_dump($session->set());

var_dump($session->max_oids);
$session->max_oids = "ttt";
$session->max_oids = 0;
var_dump($session->max_oids);
?>
--EXPECTF--
SNMP::__construct() expects at least 3 parameters, 2 given
SNMP::__construct() expects parameter 4 to be int, string given
SNMP::__construct() expects parameter 5 to be int, string given
Unknown SNMP protocol version
Exception handling

Warning: SNMP::get(): Invalid object identifier: .1.3.6.1.2.1.1.1..0 in %s on line %d
bool(false)
int(32)
string(46) "Invalid object identifier: .1.3.6.1.2.1.1.1..0"
bool(true)
Open normal session

Warning: main(): Unknown SNMP value retrieval method '67' in %s on line %d
int(%d)
Closing session

Warning: SNMP::close() expects exactly 0 parameters, 1 given in %s on line %d
bool(false)
bool(true)

Warning: SNMP::get(): Invalid or uninitialized SNMP object in %s on line %d
bool(false)
bool(true)

Warning: SNMP::walk() expects parameter 3 to be int, string given in %s on line %d
bool(false)

Warning: SNMP::walk() expects parameter 4 to be int, string given in %s on line %d
bool(false)

Warning: SNMP::get() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: SNMP::getnext() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: SNMP::set() expects exactly 3 parameters, 0 given in %s on line %d
bool(false)
NULL

Warning: main(): max_oids should be positive integer or NULL, got 0 in %s on line %d

Warning: main(): max_oids should be positive integer or NULL, got 0 in %s on line %d
NULL
