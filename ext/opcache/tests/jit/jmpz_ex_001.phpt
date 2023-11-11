--TEST--
JIT JMPZ_EX: Operand needs to be freed even if same as result
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
class Test {
    public $prop;
    public function method() {
        return $this->prop && $this->prop->method2();
    }
}

class Test2 {
    public function method2() {
        return true;
    }
};

$test = new Test;
$test->prop = new Test2;
var_dump($test->method());

?>
--EXPECT--
bool(true)
