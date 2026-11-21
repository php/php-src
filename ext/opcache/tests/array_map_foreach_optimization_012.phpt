--TEST--
array_map(): dynamic callee evaluated once on native fallback
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function plusn($x, $n) {
    return $x + $n;
}

function get_function() {
    var_dump(__FUNCTION__);
    return new class {
        function __invoke($value) {
            return $value + 1;
        }
    };
}

class C {
    function __construct() {
        var_dump(__METHOD__);
    }
    static function f($value) {
        return $value + 1;
    }
}

$array = range(1, 2);

var_dump(array_map(get_function()(...), $array));

var_dump(array_map((new C)::f(...), $array));

$f = function ($value) use (&$f) {
    $f = 'dechex';
    return $value + 1;
};

var_dump(array_map($f(...), $array));

?>
--EXPECT--
string(12) "get_function"
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
string(14) "C::__construct"
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
