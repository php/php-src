--TEST--
Bug #76466 Loop variable confusion
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
    for ($i = 0; $i < 2; $i++) {
        $field_array[] = [$i, 0];
    }
    var_dump($field_array);
}
foo();
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(0)
  }
}
