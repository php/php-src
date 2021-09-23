--TEST--
Vector setSize
--FILE--
<?php
function show(Vector $vec) {
    printf("count=%d\n", $vec->count());
    var_dump($vec->toArray());
}

$vec = new Vector();
show($vec);
$vec->setSize(2);
$vec[0] = new stdClass();
show($vec);
$vec->setSize(0);
show($vec);
?>
--EXPECT--
count=0
array(0) {
}
count=2
array(2) {
  [0]=>
  object(stdClass)#2 (0) {
  }
  [1]=>
  NULL
}
count=0
array(0) {
}
