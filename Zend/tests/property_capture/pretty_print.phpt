--TEST--
Pretty print of anon class with property capture
--INI--
zend.assertions=1
assert.exception=0
assert.warning=1
--FILE--
<?php
$a = $b = $c = $d = $e = $f = $g = $h = 42;

assert(false && new class use (
    $a,
    $b as $b_renamed,
    $c as int,
    $d as int $d_renamed,
    $e as private int,
    $f as private int $f_renamed,
    $g as readonly int,
    $h as readonly int $h_renamed,
) {});

?>
--EXPECTF--
Warning: assert(): assert(false && new class($a, $b, $c, $d, $e, $f, $g, $h) {
    public function __construct(public $a, public $b_renamed, public int $c, public int $d_renamed, private int $e, private int $f_renamed, public readonly int $g, public readonly int $h_renamed) {}
}) failed in %s on line %d