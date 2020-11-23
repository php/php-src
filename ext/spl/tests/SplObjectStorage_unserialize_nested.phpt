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
?>
--EXPECTF--
O:16:"SplObjectStorage":2:{i:0;a:4:{i:0;O:8:"stdClass":1:{s:1:"a";O:8:"stdClass":0:{}}i:1;i:1;i:2;r:4;i:3;i:2;}i:1;a:0:{}}
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
