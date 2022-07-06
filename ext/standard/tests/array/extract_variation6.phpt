--TEST--
Test extract() function (variation 6)
--FILE--
<?php

/* EXTR_REFS as second Argument */
$a = array ('foo' => 'aaa');
var_dump ( extract($a, EXTR_REFS));
var_dump($foo);

$b = $a;
$b['foo'] = 'bbb';
var_dump ( extract($a, EXTR_REFS));
var_dump($foo);
var_dump($a);

echo "Done\n";
?>
--EXPECT--
int(1)
string(3) "aaa"
int(1)
string(3) "bbb"
array(1) {
  ["foo"]=>
  &string(3) "bbb"
}
Done
