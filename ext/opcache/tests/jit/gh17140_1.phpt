--TEST--
GH-17140 (Assertion failure in JIT trace exit with ZEND_FETCH_DIM_FUNC_ARG)
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
function test() {
    $a['x'][1] = true;
    for ($fusion = 0; $i < 3; $i++) {
        var_dump($a['x'][0]);
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $i in %s on line %d

Warning: Undefined array key 0 in %s on line %d
NULL

Warning: Undefined variable $i in %s on line %d

Warning: Undefined array key 0 in %s on line %d
NULL

Warning: Undefined array key 0 in %s on line %d
NULL
