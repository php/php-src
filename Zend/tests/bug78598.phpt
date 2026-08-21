--TEST--
Bug #78598: Changing array during undef index RW error segfaults
--FILE--
<?php

$my_var = null;
set_error_handler(function() use(&$my_var) {
    $my_var = 0;
});

$my_var[0] .= "xyz";
var_dump($my_var);

$my_var = null;
$my_var[0][0][0] .= "xyz";
var_dump($my_var);

$my_var = null;
$my_var["foo"] .= "xyz";
var_dump($my_var);

$my_var = null;
$my_var["foo"]["bar"]["baz"] .= "xyz";
var_dump($my_var);

?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "xyz"
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      string(3) "xyz"
    }
  }
}
array(1) {
  ["foo"]=>
  string(3) "xyz"
}
array(1) {
  ["foo"]=>
  array(1) {
    ["bar"]=>
    array(1) {
      ["baz"]=>
      string(3) "xyz"
    }
  }
}
