--TEST--
GH-12262: Tracing JIT assertion crash when using phpstan
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_hot_func=2
--FILE--
<?php
class C {
}
trait T {
    public function equal(C $type): bool {
        return $type instanceof self && $this->value === $type->value;
    }
}
class C1 extends C {
    use T;
    public function __construct(private int $value) {
    }
}
class C2 extends C {
    use T;
	public function __construct(private string $value) {
	}
}
$x = new C1(1);
var_dump($x->equal($x));
var_dump($x->equal($x));
var_dump($x->equal($x));

$a = new C2("aaa");
var_dump($a->equal($a));
var_dump($a->equal($a));
var_dump($a->equal($a));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
