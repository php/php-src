--TEST--
BcMath\Number unserialize
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    'O:13:"BcMath\Number":1:{s:5:"value";s:1:"0";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:3:"0.0";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:1:"2";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:4:"1234";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:7:"12.0004";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:6:"0.1230";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:1:"1";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:5:"12345";}',
];

foreach ($values as $value) {
    $num = unserialize($value);
    var_dump($num);
    unset($num);
}
?>
--EXPECT--
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "0"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(3) "0.0"
  ["scale"]=>
  int(1)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "2"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(4) "1234"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(7) "12.0004"
  ["scale"]=>
  int(4)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(6) "0.1230"
  ["scale"]=>
  int(4)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(1) "1"
  ["scale"]=>
  int(0)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(5) "12345"
  ["scale"]=>
  int(0)
}
