--TEST--
JIT: FETCH_OBJ 007
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit_hot_func=2
--FILE--
<?php
class C {}

trait T {
    public function equal(C $type): bool {
        return $type instanceof self && $this->value === $type->value;
    }
}

class C1 extends C {
    use T;
    public function __construct(private int $value) {}
}

class C2 extends C {
    use T;
}

$x = new C1(1);
var_dump($x->equal($x));
var_dump($x->equal($x));
$a = new C2("aaa");
var_dump($a->equal($a));
var_dump($a->equal($a));
--EXPECTF--
bool(true)
bool(true)

Warning: Undefined property: C2::$value in %sgh15652.php on line 6

Warning: Undefined property: C2::$value in %sgh15652.php on line 6
bool(true)

Warning: Undefined property: C2::$value in %sgh15652.php on line 6

Warning: Undefined property: C2::$value in %sgh15652.php on line 6
bool(true)