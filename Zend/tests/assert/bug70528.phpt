--TEST--
Bug #70528 (assert() with instanceof adds apostrophes around class name)
--INI--
zend.assertions=1
assert.exception=0
assert.warning=1
--FILE--
<?php

namespace Foo;
class Bar {}

$bar = "Bar";
assert(new \StdClass instanceof $bar);
assert(new \StdClass instanceof Bar);
assert(new \StdClass instanceof \Foo\Bar);
?>
--EXPECTF--
Warning: assert(): assert(new \StdClass() instanceof $bar) failed in %sbug70528.php on line %d

Warning: assert(): assert(new \StdClass() instanceof Bar) failed in %sbug70528.php on line %d

Warning: assert(): assert(new \StdClass() instanceof \Foo\Bar) failed in %sbug70528.php on line %d
