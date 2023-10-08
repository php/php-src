--TEST--
OO API: Generic errors
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
} catch (ValueError $e) {
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
try {
    $session->valueretrieval = 67;
    var_dump($session->valueretrieval);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo "Closing session\n";
var_dump($session->close());

try {
    var_dump($session->get('.1.3.6.1.2.1.1.1.0'));
    var_dump($session->close());
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);

var_dump($session->max_oids);
try {
    $session->max_oids = "ttt";
} catch (TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $session->max_oids = 0;
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump($session->max_oids);
?>
--EXPECTF--
SNMP::__construct() expects at least 3 arguments, 2 given
SNMP::__construct(): Argument #4 ($timeout) must be of type int, string given
SNMP::__construct(): Argument #5 ($retries) must be of type int, string given
SNMP::__construct(): Argument #1 ($version) must be a valid SNMP protocol version
Exception handling

Warning: SNMP::get(): Invalid object identifier: .1.3.6.1.2.1.1.1..0 in %s on line %d
bool(false)
int(32)
string(46) "Invalid object identifier: .1.3.6.1.2.1.1.1..0"
bool(true)
Open normal session
SNMP retrieval method must be a bitmask of SNMP_VALUE_LIBRARY, SNMP_VALUE_PLAIN, and SNMP_VALUE_OBJECT
Closing session
bool(true)
Invalid or uninitialized SNMP object
NULL
Cannot assign string to property SNMP::$max_oids of type ?int
SNMP::$max_oids must be greater than 0 or null
NULL
