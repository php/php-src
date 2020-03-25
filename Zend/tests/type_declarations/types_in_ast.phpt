--TEST--
AST pretty-printer
--INI--
zend.assertions=1
assert.exception=0
--FILE--
<?php
assert(0 && ($a = function (int $a, ?int $b, int $c = null): ?int {
    $x = new class {
        public $a;
        public int $b;
        public ?int $c;
    };
}));
?>
--EXPECTF--
Warning: assert(): assert(0 && ($a = function (int $a, ?int $b, int $c = null): ?int {
    $x = new class {
        public $a;
        public int $b;
        public ?int $c;
    };
})) failed in %stypes_in_ast.php on line 2
