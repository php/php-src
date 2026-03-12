--TEST--
GH-19305 002: Operands may be released during comparison
--FILE--
<?php

$a = [
    'foo' => 'test',
    'bar' => 2,
];
$b = [
    'foo' => new class {
        public function __toString() {
            global $a, $b;
            $a = $b = null;
            return '';
        }
    },
    'bar' => 2,
];

// Comparison of $a['foo'] and $b['foo'] calls __toString(), which releases
// both $a and $b.
var_dump($a > $b);

?>
--EXPECT--
bool(true)
