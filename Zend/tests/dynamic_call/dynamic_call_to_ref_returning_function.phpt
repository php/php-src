--TEST--
When performing a dynamic call to a ret-by-ref function, the reference should be unwrapped
--FILE--
<?php

namespace Foo;

function &retRef($x) {
    return $x;
}

var_dump(call_user_func('Foo\retRef', 42));
var_dump(call_user_func_array('Foo\retRef', [42]));

$closure = function &($x) {
    return $x;
};
var_dump($closure->call(new class {}, 42));

var_dump((new \ReflectionFunction('Foo\retRef'))->invoke(42));
var_dump((new \ReflectionFunction('Foo\retRef'))->invokeArgs([42]));

class Bar {
    function &method($x) {
        return $x;
    }
}
var_dump((new \ReflectionMethod('Foo\Bar', 'method'))->invoke(new Bar, 42));
var_dump((new \ReflectionMethod('Foo\Bar', 'method'))->invokeArgs(new Bar, [42]));

?>
--EXPECT--
int(42)
int(42)
int(42)
int(42)
int(42)
int(42)
int(42)
