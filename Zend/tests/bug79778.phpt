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
--EXPECTF--
object(Closure)#%d (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["static"]=>
  array(1) {
    ["var"]=>
    NULL
  }
}
Closure Object
(
    [name] => {closure:%s:%d}
    [static] => Array
        (
            [var] => 
        )

)
Undefined constant "CONST_REF"
object(Closure)#%d (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["static"]=>
  array(1) {
    ["var"]=>
    NULL
  }
}
Closure Object
(
    [name] => {closure:%s:%d}
    [static] => Array
        (
            [var] => 
        )

)
object(Closure)#%d (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["static"]=>
  array(1) {
    ["var"]=>
    string(3) "foo"
  }
}
Closure Object
(
    [name] => {closure:%s:%d}
    [static] => Array
        (
            [var] => foo
        )

)
