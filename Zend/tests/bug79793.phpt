--TEST--
Bug #79793: Use after free if string used in undefined index warning is changed
--FILE--
<?php

$key = "foo";
$key .= "bar";
set_error_handler(function($_, $m) use (&$key) {
    echo "$m\n";
    $key .= "baz";
});

$ary = [];
$ary[$key]++;
var_dump($ary);
$ary[$key] += 1;
var_dump($ary);

?>
--EXPECT--
Undefined index: foobar
array(1) {
  ["foobar"]=>
  int(1)
}
Undefined index: foobarbaz
array(2) {
  ["foobar"]=>
  int(1)
  ["foobarbaz"]=>
  int(1)
}
