--TEST--
ArrayObject copied by an error handler while creating an element
--FILE--
<?php
$ao = new ArrayObject(['a' => 1], ArrayObject::ARRAY_AS_PROPS);
set_error_handler(function ($errno, $errstr) use ($ao) {
    echo $errstr, "\n";
    $GLOBALS['copies'][] = $ao->getArrayCopy();
});
$ao->b .= 'x';
$ao->c++;
$ao['d'] .= 'y';
restore_error_handler();
var_dump($ao->getArrayCopy(), $copies);
?>
--EXPECT--
Undefined array key "b"
Undefined array key "c"
Undefined array key "d"
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(1) "x"
  ["c"]=>
  int(1)
  ["d"]=>
  string(1) "y"
}
array(3) {
  [0]=>
  array(1) {
    ["a"]=>
    int(1)
  }
  [1]=>
  array(2) {
    ["a"]=>
    int(1)
    ["b"]=>
    string(1) "x"
  }
  [2]=>
  array(3) {
    ["a"]=>
    int(1)
    ["b"]=>
    string(1) "x"
    ["c"]=>
    int(1)
  }
}
