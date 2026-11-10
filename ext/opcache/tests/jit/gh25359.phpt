--TEST--
GH-25359 (Tracing JIT drops reference type of CV passed as a named by-ref argument)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_hot_func=1
--FILE--
<?php
namespace test;
try { throw new \Exception; } catch (\Exception $e) {}
function f(&$a = null, &$b = null) {}
function g($s) {
	f(b: $s);
	return $s;
}
g("x");
var_dump(strtolower(g("FALSE")));
?>
--EXPECT--
string(5) "false"
