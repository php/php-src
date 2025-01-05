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
new SNMP(SNMP::VERSION_1, "$hostname4:-1", $community, $timeout, $retries);
new SNMP(SNMP::VERSION_1, "$hostname4:65536", $community, $timeout, $retries);
new SNMP(SNMP::VERSION_1, "$longhostname:$port", $community, $timeout, $retries);
echo "OK";
?>
--EXPECTF--

Warning: SNMP::__construct(): Remote port must be between 0 and 65535 in %s on line %d

Warning: SNMP::__construct(): Remote port must be between 0 and 65535 in %s on line %d

Warning: SNMP::__construct(): hostname length must be lower than 128 in %s on line %d
OK
