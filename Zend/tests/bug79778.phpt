--TEST--
Bug #79778: Assertion failure if dumping closure with unresolved static variable
--FILE--
<?php
$closure1 = function() {
    static $var = CONST_REF;
};
var_dump($closure1);
print_r($closure1);
?>
--EXPECT--
object(Closure)#1 (1) {
  ["static"]=>
  array(1) {
    ["var"]=>
    string(14) "<constant ast>"
  }
}
Closure Object
(
    [static] => Array
        (
            [var] => <constant ast>
        )

)
