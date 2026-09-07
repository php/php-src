--TEST--
Partial application function name
--FILE--
<?php

function g($a) {}

class C {
    static function m() {
        echo "# From a method:\n";
        var_dump((new ReflectionFunction(g(?)))->getName());
    }
}

function f() {
    echo "# From a function:\n";
    var_dump((new ReflectionFunction(g(?)))->getName());

    echo "# Declared on same line:\n";
    [$a, $b] = [g(?), g(?)];
    var_dump((new ReflectionFunction($a))->getName(), (new ReflectionFunction($b))->getName());
}

f();
C::m();

echo "# From global scope:\n";
var_dump((new ReflectionFunction(g(?)))->getName());

?>
--EXPECTF--
# From a function:
string(20) "{closure:pfa:f():14}"
# Declared on same line:
string(20) "{closure:pfa:f():17}"
string(20) "{closure:pfa:f():17}"
# From a method:
string(22) "{closure:pfa:C::m():8}"
# From global scope:
string(%d) "{closure:pfa:%sfunction_name.php:25}"
