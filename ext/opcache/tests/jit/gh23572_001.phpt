--TEST--
GH-23572: Tracing JIT calls an instance method statically from trait clones
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_hot_func=2
opcache.jit_max_polymorphic_calls=0
--FILE--
<?php
trait T {
    public function callSelf() {
        return self::method();
    }
}

class A {
    use T;

    public static function method() {
        return 'A';
    }
}

class B {
    use T;

    public function method() {
        var_dump($this);
        return 'B';
    }
}

$a = new A;
$b = new B;

for ($i = 0; $i < 4; $i++) var_dump($a->callSelf());
for ($i = 0; $i < 4; $i++) var_dump($b->callSelf());
var_dump($a->callSelf());
?>
--EXPECT--
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
object(B)#%d (0) {
}
string(1) "B"
object(B)#%d (0) {
}
string(1) "B"
object(B)#%d (0) {
}
string(1) "B"
object(B)#%d (0) {
}
string(1) "B"
string(1) "A"
