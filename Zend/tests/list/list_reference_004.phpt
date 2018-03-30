--TEST--
"Reference Unpacking - Foreach" list()
--FILE--
<?php
$coords = array(array(1, 2), array(3, 4));
foreach ($coords as [&$x, $y]) {
    $x++;
    $y++;
}
var_dump($coords);
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    &int(4)
    [1]=>
    int(4)
  }
}
