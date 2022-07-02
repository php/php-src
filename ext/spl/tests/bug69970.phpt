--TEST--
Bug #69970 (Use-after-free vulnerability in spl_recursive_it_move_forward_ex())
--FILE--
<?php

$count = 10;

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator {
  function rewind(): void {
    echo "dummy\n";
  }
  function endChildren(): void {
      global $count;
      echo $this->getDepth();
      if (--$count > 0) {
          // Trigger use-after-free
          parent::rewind();
      }
  }
}
$arr = array("a", array("ba", array("bba", "bbb")));
$obj = new RecursiveArrayIterator($arr);
$rit = new RecursiveArrayIteratorIterator($obj);

foreach ($rit as $k => $v) {
  echo ($rit->getDepth()) . "$k=>$v\n";
}
?>
--EXPECT--
dummy
00=>a
00=>a
10=>ba
20=>bba
21=>bbb
21010=>ba
20=>bba
21=>bbb
21010=>ba
20=>bba
21=>bbb
21010=>ba
20=>bba
21=>bbb
21
