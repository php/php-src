--TEST--
Bug #29566 (foreach/string handling strangeness)
--FILE--
<?php
$var="This is a string";

$dummy="";
unset($dummy);

foreach($var['nosuchkey'] as $v) {
}
?>
===DONE===
--EXPECTF--
Warning: Illegal string offset 'nosuchkey' in %sbug29566.php on line %d

Warning: Invalid argument supplied for foreach() in %sbug29566.php on line %d
===DONE===
