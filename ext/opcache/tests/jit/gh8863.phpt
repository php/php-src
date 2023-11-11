--TEST--
Bug GH-8863: RW operation on readonly property doesn't throw with JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
class Test {
    public readonly int $prop;

    public function __construct() {
        $this->prop = 1;
    }

    public function rw() {
        $this->prop += 1;
        echo "Done\n";
    }
}

$test = new Test();
try {
    $test->rw();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
DONE
--EXPECT--
Cannot modify readonly property Test::$prop
DONE
