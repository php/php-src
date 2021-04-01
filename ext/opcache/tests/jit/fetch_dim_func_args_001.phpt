--TEST--
JIT FETCH_DIM_FUNC_ARG: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--EXTENSIONS--
opcache
--FILE--
<?php
namespace A;

class A {
    public function change(array $config) {
        $config['keys'] = array_keys($config["a"]);;
    }
}

$a = new A();

$a->change($a = array("a" => range(1, 5)));
?>
okey
--EXPECT--
okey
