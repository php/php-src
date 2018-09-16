--TEST--
Function snmp2_getnext
--CREDITS--
Olivier Doucet & Boris Lytochkin
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

echo "Single OID\n";
var_dump(snmp2_getnext($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries));
echo "Single OID in array\n";
var_dump(snmp2_getnext($hostname, $community, array('.1.3.6.1.2.1.1.1.0'), $timeout, $retries));
echo "Multiple OID\n";
var_dump(snmp2_getnext($hostname, $community, array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.6.0'), $timeout, $retries));

?>
--EXPECTF--
Single OID
string(%d) "%s"
Single OID in array
array(1) {
  ["%s"]=>
  string(%d) "%s"
}
Multiple OID
array(2) {
  ["%s"]=>
  string(%d) "%s"
  ["%s"]=>
  string(%d) "%d"
}
