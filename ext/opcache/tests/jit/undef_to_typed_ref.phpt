--TEST--
Assign undef var to typed reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
class Test {
    public string $x;
}

function test_simple_res() {
    $test = new Test;
    $test->x = "";
    $r =& $test->x;
    var_dump($r = $v);
}

function test_simple_nores() {
    $test = new Test;
    $test->x = "";
    $r =& $test->x;
    $r = $v;
}

function test_dim_res() {
    $test = new Test;
    $test->x = "";
    $a[0] =& $test->x;
    var_dump($a[0] = $v);
}

function test_dim_nores() {
    $test = new Test;
    $test->x = "";
    $a[0] =& $test->x;
    $a[0] = $v;
}

function test_obj_res() {
    $test = new Test;
    $test->x = "";
    $o = new stdClass;
    $o->p =& $test->x;
    var_dump($o->p = $v);
}

function test_obj_nores() {
    $test = new Test;
    $test->x = "";
    $o = new stdClass;
    $o->p =& $test->x;
    $o->p = $v;
}

try {
    test_simple_res();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test_simple_nores();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test_dim_res();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test_dim_nores();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test_obj_res();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test_obj_nores();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $v in %s on line %d
Cannot assign null to reference held by property Test::$x of type string

Warning: Undefined variable $v in %s on line %d
Cannot assign null to reference held by property Test::$x of type string

Warning: Undefined variable $v in %s on line %d
Cannot assign null to reference held by property Test::$x of type string

Warning: Undefined variable $v in %s on line %d
Cannot assign null to reference held by property Test::$x of type string

Warning: Undefined variable $v in %s on line %d
Cannot assign null to reference held by property Test::$x of type string

Warning: Undefined variable $v in %s on line %d
Cannot assign null to reference held by property Test::$x of type string
