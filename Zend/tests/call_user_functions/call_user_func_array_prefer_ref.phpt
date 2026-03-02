--TEST--
call_user_func_array() passes value to prefer-ref arg if element wasn't a reference
--FILE--
<?php

namespace {
    call_user_func_array('array_multisort', [[3, 2, 1]]);

    $args = [[3, 2, 1]];
    call_user_func_array('array_multisort', $args);
    var_dump($args);
    unset($args);

    $array = [3, 2, 1];
    call_user_func('array_multisort', $array);
    var_dump($array);
    unset($array);
}

namespace Foo {
    call_user_func_array('array_multisort', [[3, 2, 1]]);

    $args = [[3, 2, 1]];
    call_user_func_array('array_multisort', $args);
    var_dump($args);
    unset($args);

    $array = [3, 2, 1];
    call_user_func('array_multisort', $array);
    var_dump($array);
    unset($array);
}

?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(3)
    [1]=>
    int(2)
    [2]=>
    int(1)
  }
}
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(3)
    [1]=>
    int(2)
    [2]=>
    int(1)
  }
}
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
}
