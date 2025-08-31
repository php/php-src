--TEST--
Bug #66251 (Constants get statically bound at compile time when Optimized)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
printf ("A=%s\n", getA());
const A="hello";
function getA() {return A;}
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "A" in %s:%d
Stack trace:
#0 %s(%d): getA()
#1 {main}
  thrown in %s on line %d
