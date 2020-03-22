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
--EXPECTF--
Warning: Illegal string offset 'nosuchkey' in %sbug29566.php on line %d

Warning: foreach() argument must be of type array|object, string given in %sbug29566.php on line %d
