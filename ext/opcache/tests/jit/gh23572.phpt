--TEST--
GH-23572: Tracing JIT calls the wrong self/parent static method from trait clones
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_hot_func=2
opcache.jit_hot_loop=255
opcache.jit_hot_return=255
opcache.jit_hot_side_exit=255
--FILE--
<?php
class ParentA {
    public static function method() {
        return 'parent A';
    }
}

class ParentB {
    public static function method() {
        return 'parent B';
    }
}

trait T {
    public function callParent() {
        return parent::method();
    }

    public function callSelf() {
        return self::method();
    }
}

class A extends ParentA {
    use T;

    public static function method() {
        return 'self A';
    }
}

class B extends ParentB {
    use T;

    public static function method() {
        return 'self B';
    }
}

$a = new A;
$b = new B;

for ($i = 0; $i < 4; $i++) {
    var_dump($a->callParent());
}
var_dump($b->callParent());
var_dump($b->callParent());
var_dump($a->callParent());

for ($i = 0; $i < 4; $i++) {
    var_dump($a->callSelf());
}
var_dump($b->callSelf());
var_dump($b->callSelf());
var_dump($a->callSelf());
?>
--EXPECT--
string(8) "parent A"
string(8) "parent A"
string(8) "parent A"
string(8) "parent A"
string(8) "parent B"
string(8) "parent B"
string(8) "parent A"
string(6) "self A"
string(6) "self A"
string(6) "self A"
string(6) "self A"
string(6) "self B"
string(6) "self B"
string(6) "self A"
