--TEST--
AST pretty-printer for nullable casts
--INI--
zend.assertions=1
assert.exception=0
--FILE--
<?php
assert(0 && (?bool)(?boolean) ( ?int )(?integer) (? float)(?double)(?real) ( ? string ) (?array) (?object) $x);
?>
--EXPECTF--
Warning: assert(): assert(0 && (?bool)((?bool)((?int)((?int)((?double)((?double)((?double)((?string)((?array)((?object)$x)))))))))) failed in %s on line 2
