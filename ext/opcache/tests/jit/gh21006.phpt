--TEST--
GH-21006: JIT SEGV with FETCH_OBJ_FUNC_ARG and property hooks
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
namespace Test;

class C
{
    public $prop {
        get => 'sha256';
    }

    public function sign()
    {
        return hash_hmac(
            algo: $this->prop,
            data: '',
            key: '',
        );
    }
}

$obj = new C();
for ($i = 0; $i < 100; $i++) {
    $obj->sign();
}

#[\AllowDynamicProperties]
class D
{
    public function test()
    {
        return hash_hmac(
            algo: $this->algo,
            data: '',
            key: '',
        );
    }
}

$d = new D();
$d->algo = 'sha256';
for ($i = 0; $i < 100; $i++) {
    $d->test();
}

class E
{
    public $prop {
        get => [];
    }

    public function sign()
    {
        (new Trampoline)->f($this->prop);
    }
}

class Trampoline
{
    public function __call($name, $args) {}
}

$e = new E();
for ($i = 0; $i < 100; $i++) {
    $e->sign();
}
echo "OK\n";
?>
--EXPECT--
OK
