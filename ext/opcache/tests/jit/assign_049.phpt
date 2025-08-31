--TEST--
JIT ASSIGN: register allocation on x86
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function &a($i) {
    $a = "str". $i;
    return $a;
}

class A {
    public $a;
    public function test() {
        $this->a = a(1);
    }
}

$a = new A;
$a->test();
$a->test();
?>
DONE
--EXPECT--
DONE
