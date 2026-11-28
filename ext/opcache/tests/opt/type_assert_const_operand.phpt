--TEST--
TYPE_ASSERT does not consume its operand, which may be replaced by a constant
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
/* The array_map() loop uses the array in TYPE_ASSERT and then in FE_RESET_R.
 * The array is only known to be constant after optimization. */
function map_array() {
    return array_map(strlen(...), constant('PHP_EOL') ? ['a', 'bb'] : 1);
}
function map_int() {
    return array_map(strlen(...), constant('PHP_EOL') ? 1 : []);
}

var_dump(map_array());
try {
    map_int();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array_map(): Argument #2 ($array) must be of type array, int given
