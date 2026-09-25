--TEST--
DCE must not keep the replaced instruction's extended_value on a synthesized FREE
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
opcache.jit=disable
--EXTENSIONS--
opcache
--FILE--
<?php
class D {
    public function __destruct() {
        throw new Exception("dtor");
    }
}

final class C {
    public array $a;

    public function __construct() {
        $this->a = [new D];
    }

    public function clobber(): int {
        $this->a = [];
        return 0;
    }

    public function test() {
        empty(($this->a ?? [])[$this->clobber()]);
    }
}

$c = new C;
try {
    $c->test();
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

$a = [];
for ($i = 0; $i < 10; $i++) {
    $a[] = [$i];
}
echo count($a), "\n";
?>
--EXPECT--
Exception: dtor
10
