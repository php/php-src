--TEST--
GH-18304 (Changing the properties of a DateInterval through dynamic properties triggers a SegFault)
--CREDITS--
orose-assetgo
--EXTENSIONS--
snmp
--FILE--
<?php
$snmp = new SNMP(1, '127.0.0.1', 'community');
$field = 'max_oids';
$snmp->$field++;
var_dump($snmp->$field);
?>
--EXPECT--
int(1)
