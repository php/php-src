--TEST--
GH-21006: JIT SEGV with FETCH_OBJ_FUNC_ARG and property hooks
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_hot_loop=61
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
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

// Dynamic property access through FETCH_OBJ_FUNC_ARG must not corrupt
// the ZEND_DYNAMIC_PROPERTY_OFFSET sentinel in the runtime cache.
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
echo "OK\n";
?>
--EXPECT--
OK
