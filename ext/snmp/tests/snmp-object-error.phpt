--TEST--
OO API: Generic errors
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
var_dump($session->close());
var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
var_dump($session->close());

$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);

var_dump($session->max_oids);
$session->max_oids = "ttt";
$session->max_oids = 0;
var_dump($session->max_oids);
?>
--EXPECTF--
SNMP::__construct() expects at least 3 parameters, 2 given
SNMP::__construct(): Argument #4 must be of type int, string given
SNMP::__construct(): Argument #5 must be of type int, string given
SNMP::__construct(): Argument #1 ($version) must be a valid SNMP protocol version
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
bool(true)

Warning: SNMP::get(): Invalid or uninitialized SNMP object in %s on line %d
bool(false)
bool(true)
NULL

Warning: main(): max_oids should be positive integer or NULL, got 0 in %s on line %d

Warning: main(): max_oids should be positive integer or NULL, got 0 in %s on line %d
NULL
