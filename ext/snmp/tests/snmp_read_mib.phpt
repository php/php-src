--TEST--
Function snmp_read_mib
--CREDITS--
Olivier Doucet Olivier Doucet Boris Lytochkin
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
require_once(dirname(__FILE__).'/snmp_include.inc');

if (!file_exists($mibdir . '/SNMPv2-MIB.txt')) die('skip MIB file not in the system');

?>
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

echo "Checking error handling\n";
var_dump(snmp_read_mib());
var_dump(snmp_read_mib(dirname(__FILE__).'/cannotfindthisfile'));

echo "Checking working\n";
var_dump(snmp_read_mib($mibdir . '/SNMPv2-MIB.txt'));

?>
--EXPECTF--
Checking error handling

Warning: snmp_read_mib() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: snmp_read_mib(): Error while reading MIB file '%s': No such file or directory in %s on line %d
bool(false)
Checking working
bool(true)
