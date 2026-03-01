--TEST--
IPv6 support
--CREDITS--
Boris Lytochkin
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');

$packed = str_repeat(chr(0), 15) . chr(1);
if (@inet_ntop($packed) === false) {
    die("skip no IPv6 support");
}
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

var_dump(snmpget($hostname6_port, $community, '.1.3.6.1.2.1.1.1.0'));
try {
    var_dump(snmpget('[dead:beef::', $community, '.1.3.6.1.2.1.1.1.0'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
string(%d) "%s"
Malformed IPv6 address, closing square bracket missing
