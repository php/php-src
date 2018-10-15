--TEST--
SPL: Test unserializing tested & linked storage
--FILE--
<?php
$o = new StdClass();
$a = new StdClass();

$o->a = $a;

$so = new SplObjectStorage();

$so[$o] = 1;
$so[$a] = 2;

$s = serialize($so);
echo $s."\n";

$so1 = unserialize($s);
var_dump($so1);
--EXPECTF--
C:16:"SplObjectStorage":76:{x:i:2;O:8:"stdClass":1:{s:1:"a";O:8:"stdClass":0:{}},i:1;;r:4;,i:2;;m:a:0:{}}
object(SplObjectStorage)#4 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(2) {
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#5 (1) {
        ["a"]=>
        object(stdClass)#6 (0) {
        }
      }
      ["inf"]=>
      int(1)
    }
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#6 (0) {
      }
      ["inf"]=>
      int(2)
    }
  }
}
