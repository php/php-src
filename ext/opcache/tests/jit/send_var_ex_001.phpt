--TEST--
JIT SEND_VAR_EX fails on SHOULD_SEND_BY_REF checking
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
namespace A;

class A {
    private $evalParameters;
    public function evaluate() {
        $this->evalParameters = array("a" => "okey");
        extract($this->evalParameters, EXTR_SKIP);
        echo $a;
        return false;
    }
}

$a = new A();

$a->evaluate();
?>
--EXPECT--
okey
