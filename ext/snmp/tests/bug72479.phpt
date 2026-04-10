--TEST--
Bug #72479: Use After Free Vulnerability in SNMP with GC and unserialize()
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
$arr = [1, [1, 2, 3, 4, 5], 3, 4, 5];
$poc = 'a:3:{i:1;N;i:2;O:4:"snmp":1:{s:11:"quick_print";'.serialize($arr).'}i:1;R:7;}';
try {
    $out = unserialize($poc);
    var_dump($out);
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Unserialization of 'SNMP' is not allowed
