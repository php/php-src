--TEST--
Collections\Deque clear
--FILE--
<?php

$it = new Collections\Deque(['first' => new stdClass()]);
var_dump($it->toArray());
printf("count=%d\n", $it->count());
$it->clear();
foreach ($it as $value) {
    echo "Not reached\n";
}
var_dump($it->toArray());
printf("count=%d\n", $it->count());
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
count=1
array(0) {
}
count=0