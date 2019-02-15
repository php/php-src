--TEST--
IPv6 support
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');

$packed = str_repeat(chr(0), 15) . chr(1);
if (@inet_ntop($packed) === false) {
	die("skip no IPv6 support");
}
?>
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

var_dump(snmpget($hostname6_port, $community, '.1.3.6.1.2.1.1.1.0'));
var_dump(snmpget('[dead:beef::', $community, '.1.3.6.1.2.1.1.1.0'));
?>
--EXPECTF--
string(%d) "%s"

Warning: snmpget(): malformed IPv6 address, closing square bracket missing in %s on line %d
bool(false)
