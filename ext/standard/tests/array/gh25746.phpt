--TEST--
GH-25746: array_column() crashes with JIT when the result for an empty array is modified
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
function test(array $rows) {
    $ids = array_column($rows, "id");
    $ids[] = 1;
    return $ids;
}
var_dump(test([]));
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
