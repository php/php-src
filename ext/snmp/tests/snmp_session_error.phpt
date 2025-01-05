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
echo "OK";
?>
--EXPECT--
remote port must be between 0 and 65535
remote port must be between 0 and 65535
hostname length must be lower than 128
OK
