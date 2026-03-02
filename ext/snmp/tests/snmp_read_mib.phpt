--TEST--
Function snmp_read_mib
--CREDITS--
Olivier Doucet Olivier Doucet Boris Lytochkin
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
require_once(__DIR__.'/snmp_include.inc');

if (!file_exists($mibdir . '/SNMPv2-MIB.txt')) die('skip MIB file not in the system');

?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking error handling\n";
var_dump(snmp_read_mib(__DIR__.'/cannotfindthisfile'));

echo "Checking working\n";
var_dump(snmp_read_mib($mibdir . '/SNMPv2-MIB.txt'));

?>
--EXPECTF--
Checking error handling

Warning: snmp_read_mib(): Error while reading MIB file '%s': No such file or directory in %s on line %d
bool(false)
Checking working
bool(true)
