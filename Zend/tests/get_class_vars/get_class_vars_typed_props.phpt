--TEST--
get_class_vars() returns uninitialized typed properties with a null value
--FILE--
<?php

class Test {
    public static int $int1;
    public static int $int2 = 42;
    public int $int3;
    public int $int4 = 42;
}

var_dump(get_class_vars(Test::class));

?>
--EXPECT--
array(4) {
  ["int3"]=>
  NULL
  ["int4"]=>
  int(42)
  ["int1"]=>
  NULL
  ["int2"]=>
  int(42)
}
