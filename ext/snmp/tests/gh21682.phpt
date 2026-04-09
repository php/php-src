--TEST--
GH-21682 (SNMP should not be serializable)
--EXTENSIONS--
snmp
--FILE--
<?php
$s = new SNMP(SNMP::VERSION_1, "localhost", "public");
try {
    serialize($s);
    echo "ERROR: should have thrown\n";
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}
$s->close();
?>
--EXPECT--
Serialization of 'SNMP' is not allowed
