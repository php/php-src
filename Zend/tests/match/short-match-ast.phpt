--TEST--
Short-hand match with implicit true subject, AST printing.
--INI--
assert.exception=0
--FILE--
<?php

assert((function () {
    $a = 3;
    match {
      $a < 2 => 'small',
      $a == 3 => 'medium',
      default => 'large',
    };
})());

?>
--EXPECTF--
Warning: assert(): assert(function () {
    $a = 3;
    match {
        $a < 2 => 'small',
        $a == 3 => 'medium',
        default => 'large',
    };
}()) failed in %s on line %d
