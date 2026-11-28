--TEST--
GH-25858 (snmp_init_mib() dereferences NULL when $mibdirs is omitted or null)
--CREDITS--
OmenP007
--EXTENSIONS--
snmp
--FILE--
<?php
snmp_init_mib();
snmp_init_mib(null);
echo "Done\n";
?>
--EXPECT--
Done
