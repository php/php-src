--TEST--
Bug #75921: Inconsistent error when creating stdObject from empty variable
--FILE--
<?php

$null->a = 42;
var_dump($null);
unset($null);

$null->a['hello'] = 42;
var_dump($null);
unset($null);

$null->a->b = 42;
var_dump($null);
unset($null);

$null->a['hello']->b = 42;
var_dump($null);
unset($null);

$null->a->b['hello'] = 42;
var_dump($null);
unset($null);

?>
--EXPECTF--
Warning: Creating default object from empty value in %sbug75921.php on line 3
object(stdClass)#1 (1) {
  ["a"]=>
  int(42)
}

Warning: Creating default object from empty value in %sbug75921.php on line 7
object(stdClass)#1 (1) {
  ["a"]=>
  array(1) {
    ["hello"]=>
    int(42)
  }
}

Warning: Creating default object from empty value in %sbug75921.php on line 11

Warning: Creating default object from empty value in %sbug75921.php on line 11
object(stdClass)#1 (1) {
  ["a"]=>
  object(stdClass)#2 (1) {
    ["b"]=>
    int(42)
  }
}

Warning: Creating default object from empty value in %sbug75921.php on line 15

Warning: Creating default object from empty value in %sbug75921.php on line 15
object(stdClass)#1 (1) {
  ["a"]=>
  array(1) {
    ["hello"]=>
    object(stdClass)#2 (1) {
      ["b"]=>
      int(42)
    }
  }
}

Warning: Creating default object from empty value in %sbug75921.php on line 19

Warning: Creating default object from empty value in %sbug75921.php on line 19
object(stdClass)#1 (1) {
  ["a"]=>
  object(stdClass)#2 (1) {
    ["b"]=>
    array(1) {
      ["hello"]=>
      int(42)
    }
  }
}
