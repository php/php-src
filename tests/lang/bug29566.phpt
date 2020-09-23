--TEST--
Bug #29566 (foreach/string handling strangeness)
--FILE--
<?php
$var="This is a string";

$dummy="";
unset($dummy);

try {
    foreach($var['0nosuchkey'] as $v) {}
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECTF--
Warning: Illegal string offset "0nosuchkey" in %s on line %d
foreach() argument must be of type array|object, string given
