--TEST--
Vector setSize
--FILE--
<?php
function show(Vector $it) {
    printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
    var_dump($it->toArray());
}

$it = new Vector();
show($it);
$it->setSize(2);
$it[0] = new stdClass();
show($it);
$it->setSize(0);
show($it);
?>
--EXPECT--
count=0 capacity=0
array(0) {
}
count=2 capacity=2
array(2) {
  [0]=>
  object(stdClass)#2 (0) {
  }
  [1]=>
  NULL
}
count=0 capacity=0
array(0) {
}
