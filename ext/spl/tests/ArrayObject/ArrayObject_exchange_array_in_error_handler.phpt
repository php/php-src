--TEST--
ArrayObject storage exchanged by an error handler while creating an element
--FILE--
<?php
$ao = new ArrayObject(['a' => 1], ArrayObject::ARRAY_AS_PROPS);
set_error_handler(function ($errno, $errstr) use ($ao) {
    echo $errstr, "\n";
    $ao->exchangeArray(['z' => str_repeat('z', 3)]);
});
$ao->b .= 'x';
restore_error_handler();
var_dump($ao->getArrayCopy());
?>
--EXPECT--
Undefined array key "b"
array(2) {
  ["z"]=>
  string(3) "zzz"
  ["b"]=>
  string(1) "x"
}
