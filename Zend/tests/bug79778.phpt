--TEST--
Bug #79778: Assertion failure if dumping closure with unresolved static variable
--FILE--
<?php
$closure1 = function() {
    static $var = CONST_REF;
};

var_dump($closure1);
print_r($closure1);

try {
    $closure1();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($closure1);
print_r($closure1);

const CONST_REF = 'foo';
$closure1();
var_dump($closure1);
print_r($closure1);

?>
--EXPECT--
object(Closure)#1 (1) {
  ["static"]=>
  array(1) {
    ["var"]=>
    NULL
  }
}
Closure Object
(
    [static] => Array
        (
            [var] => 
        )

)
Undefined constant "CONST_REF"
object(Closure)#1 (1) {
  ["static"]=>
  array(1) {
    ["var"]=>
    NULL
  }
}
Closure Object
(
    [static] => Array
        (
            [var] => 
        )

)
object(Closure)#1 (1) {
  ["static"]=>
  array(1) {
    ["var"]=>
    string(3) "foo"
  }
}
Closure Object
(
    [static] => Array
        (
            [var] => foo
        )

)
