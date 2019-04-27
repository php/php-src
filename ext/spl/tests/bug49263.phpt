--TEST--
SPL: SplObjectStorage serialization references
--FILE--
<?php
$o1 = new stdClass;
$o2 = new stdClass;

$s = new splObjectStorage();

$s->attach($o1, array('prev' => 2, 'next' => $o2));
$s->attach($o2, array('prev' => $o1));

$ss = serialize($s);
unset($s,$o1,$o2);
echo $ss."\n";
var_dump(unserialize($ss));
?>
===DONE===
--EXPECTF--
O:16:"SplObjectStorage":2:{i:0;a:4:{i:0;O:8:"stdClass":0:{}i:1;a:2:{s:4:"prev";i:2;s:4:"next";O:8:"stdClass":0:{}}i:2;r:6;i:3;a:1:{s:4:"prev";r:3;}}i:1;a:0:{}}
object(SplObjectStorage)#2 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(2) {
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#1 (0) {
      }
      ["inf"]=>
      array(2) {
        ["prev"]=>
        int(2)
        ["next"]=>
        object(stdClass)#3 (0) {
        }
      }
    }
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#3 (0) {
      }
      ["inf"]=>
      array(1) {
        ["prev"]=>
        object(stdClass)#1 (0) {
        }
      }
    }
  }
}
===DONE===
