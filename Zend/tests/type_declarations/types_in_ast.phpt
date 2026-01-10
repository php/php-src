--TEST--
AST pretty-printer
--INI--
zend.assertions=1
--FILE--
<?php
try {
assert(0 && ($a = function (int $a, ?int $b, int $c = null): ?int {
    $x = new class {
        public $a;
        public int $b;
        public ?int $c;
    };
}));
} catch (AssertionError $e) {
    echo 'assert(): ', $e->getMessage(), ' failed', PHP_EOL;
}
?>
--EXPECT--
assert(): assert(0 && ($a = function (int $a, ?int $b, int $c = null): ?int {
    $x = new class {
        public $a;
        public int $b;
        public ?int $c;
    };
})) failed
