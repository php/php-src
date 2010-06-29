--TEST--
SPL: Test unserializing tested & linked storage
--FILE--
<?php
$o = new StdClass();
$a = new StdClass();

$o->a = $a;

$so = new SplObjectStorage();

$so->attach($o);
$so->attach($a);

$s = serialize($so);
echo $s."\n";

$so1 = unserialize($s);
var_dump($so1);
foreach($so1 as $obj) {
  var_dump($obj);
}
--EXPECTF--
C:16:"SplObjectStorage":66:{x:i:2;O:8:"stdClass":1:{s:1:"a";O:8:"stdClass":0:{}};r:2;;m:a:0:{}}
object(SplObjectStorage)#4 (0) {
}
object(stdClass)#5 (1) {
  ["a"]=>
  object(stdClass)#6 (0) {
  }
}
object(stdClass)#6 (0) {
}
