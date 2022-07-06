--TEST--
Test with optimization of function calls
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

class A {
    public $obj;
    public function test($a) {
    }
}

function a(&$b) {
    $b = "changed";
    return "done";
}

$a = "a";
$b = "b";
$c = "c";
$f = "a";

/*
 * INIT_FCALL_BY_NAME
 * SEND_VAR
 * DO_FCALL
 * DO_FCALL_BY_NAME
 */
foo(a($a));
var_dump($a);
$a = "a";

/*
 * INIT_FCALL_BY_NAME
 *   INIT_FCALL_BY_NAME -- un-optimizable
 *   DO_FCALL_BY_NAME   -- un-optimizable
 * DO_FCALL_BY_NAME
 */
foo($f($a));
var_dump($a);

/*
 * INIT_FCALL_BY_NAME
 *   ZEND_NEW
 *   DO_FCALL_BY_NAME
 * DO_FCALL_BY_NAME
 */
foo(new A());

/*
 * INIT_FCALL_BY_NAME
 * FETCH_OBJ_FUNC_ARG
 * ZEND_SEND_VAR
 * DO_FCALL_BY_NAME
 */
foo((new A)->obj);
$obj = new A;
ref($obj->obj);
var_dump($obj->obj);

ref(retarray()[0]);

$a = "a";
foo(a($a), $a, ref($b, $c), $obj);
var_dump($a);
var_dump($b);

/*
 * INIT_FCALL_BY_NAME
 * SEND_VAL
 * DO_FCALL_BY_NAME
 */
ref("xxx");

function retarray() {
    return array("retarray");
}

function foo($a) {
    print_r(func_get_args());
}

function ref(&$b) {
    $b = "changed";
    return "ref";
}
?>
--EXPECTF--
Array
(
    [0] => done
)
string(7) "changed"
Array
(
    [0] => done
)
string(7) "changed"
Array
(
    [0] => A Object
        (
            [obj] => 
        )

)
Array
(
    [0] => 
)
string(7) "changed"
Array
(
    [0] => done
    [1] => changed
    [2] => ref
    [3] => A Object
        (
            [obj] => changed
        )

)
string(7) "changed"
string(7) "changed"

Fatal error: Uncaught Error: ref(): Argument #1 ($b) cannot be passed by reference in %soptimize_func_calls.php:%d
Stack trace:
#0 {main}
  thrown in %soptimize_func_calls.php on line %d
