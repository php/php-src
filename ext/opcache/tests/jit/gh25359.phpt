--TEST--
GH-25359 (Tracing JIT drops reference type of CV passed as a named by-ref argument)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_hot_func=1
--FILE--
<?php
namespace Test;
function f(&$a = null, &$b = null) { $b++; }
function g(int $s) {
	f(b: $s);
	return $s + 1;
}
for ($i = 0; $i < 5; $i++) {
	var_dump(g(41));
}
?>
--EXPECT--
int(43)
int(43)
int(43)
int(43)
int(43)
