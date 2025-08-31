--TEST--
Bug #74019	(Segfault with list)
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php

class A {
    public function seg() {
        list($a, $b) = A::CONSTS;
        var_dump($a, $b);
        return;
    }
    const CONSTS = [1, 2];
}

$a = new A;
$a->seg();
?>
--EXPECT--
int(1)
int(2)
