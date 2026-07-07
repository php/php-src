--TEST--
Markup syntax: a mismatched <slot:> closing tag is a compile error (RFC §7)
--EXTENSIONS--
html
--FILE--
<?php
function C(#[Html\Slot('a')] ?Html\Htmlable $a = null): Html\Htmlable { return new Html\Fragment([$a]); }
$x = <C><slot:a>oops</slot:b></C>;
?>
--EXPECTF--
Fatal error: Mismatched markup closing tag: expected </slot:a>, found </slot:b> in %s on line %d
