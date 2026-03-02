--TEST--
BcMath\Number sqrt()
--EXTENSIONS--
bcmath
--FILE--
<?php

$radicants = [
    "15151324141414.412312232141241",
    "141241241241241248267654747412",
    "0.1322135476547459213732911312",
];

foreach ($radicants as $radicant) {
    $method_ret = (new BcMath\Number($radicant))->sqrt();
    var_dump($method_ret);
}
?>
--EXPECT--
object(BcMath\Number)#2 (2) {
  ["value"]=>
  string(33) "3892470.1850385973524458288799178"
  ["scale"]=>
  int(25)
}
object(BcMath\Number)#3 (2) {
  ["value"]=>
  string(26) "375820756799356.7439216735"
  ["scale"]=>
  int(10)
}
object(BcMath\Number)#1 (2) {
  ["value"]=>
  string(40) "0.36361180901442945486553617684111499023"
  ["scale"]=>
  int(38)
}
