--TEST--
GH-11171: Test
--FILE--
<?php
$all = ['test'];
foreach ($all as &$item) {
    $all += [$item];
}
var_dump($all);
?>
--EXPECT--
array(1) {
  [0]=>
  &string(4) "test"
}
