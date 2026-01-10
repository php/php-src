--TEST--
SNMP::__construct checks
--CREDITS--
Boris Lytochkin
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

$longhostname = str_repeat($hostname4, 1_000_000);
try {
	new SNMP(SNMP::VERSION_1, "$hostname4:-1", $community, $timeout, $retries);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	new SNMP(SNMP::VERSION_1, "$hostname4:65536", $community, $timeout, $retries);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	new SNMP(SNMP::VERSION_1, "$longhostname:$port", $community, $timeout, $retries);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	new SNMP(SNMP::VERSION_1, "$hostname:$port", $community, PHP_INT_MIN, $retries);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	new SNMP(SNMP::VERSION_1, "$hostname:$port", $community, $timeout, PHP_INT_MAX);
	echo PHP_INT_SIZE, "\n"; // no exception on 32bit machines
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	new SNMP(SNMP::VERSION_1, "\0$hostname:$port", $community, $timeout, $retries);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	new SNMP(SNMP::VERSION_1, "$hostname:$port", "", $timeout, $retries);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	new SNMP(SNMP::VERSION_1, "$hostname:$port", "\0$community", $timeout, $retries);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
echo "OK";
?>
--EXPECTF--
SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
SNMP::__construct(): Argument #2 ($hostname) remote port must be between 0 and 65535
SNMP::__construct(): Argument #2 ($hostname) length must be lower than 128
SNMP::__construct(): Argument #4 ($timeout) must be between -1 and %d
%r(SNMP::__construct\(\): Argument #5 \(\$retries\) must be between -1 and %d|4)%r
SNMP::__construct(): Argument #2 ($hostname) must not contain any null bytes
SNMP::__construct(): Argument #3 ($community) must not be empty
SNMP::__construct(): Argument #3 ($community) must not contain any null bytes
OK
