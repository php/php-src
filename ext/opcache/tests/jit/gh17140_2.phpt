--TEST--
GH-17140 (Assertion failure in JIT trace exit with ZEND_FETCH_OBJ_FUNC_ARG)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1254
opcache.jit_buffer_size=32M
opcache.jit_hot_func=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
namespace Foo;
class X {
        public $a = 1;
        public $b;
        function __construct() {
                unset($this->b);
        }
}
function test() {
    $a['x'] = new X;
    for ($fusion = 0; $i < 3; $i++) {
        var_dump($a['x']->b);
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $i in %s on line %d

Warning: Undefined property: Foo\X::$b in %s on line %d
NULL

Warning: Undefined variable $i in %s on line %d

Warning: Undefined property: Foo\X::$b in %s on line %d
NULL

Warning: Undefined property: Foo\X::$b in %s on line %d
NULL
