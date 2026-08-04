--TEST--
GH-21134: ASan negative-size-param in preg_match_all() with \C + UTF-8 multibyte input
--CREDITS--
vi3tL0u1s
--FILE--
<?php

$r = preg_match_all("/(.*)\\C/u", "à", $m);
var_dump($r, $m);

?>
--EXPECTF--
Warning: preg_match_all(): Compilation failed: using \C is incompatible with the 'u' modifier at offset 6 in %s on line %d
bool(false)
NULL
