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
assert(new \stdClass instanceof $bar);
assert(new \stdClass instanceof Bar);
assert(new \stdClass instanceof \Foo\Bar);
?>
--EXPECTF--
Warning: assert(): assert(new \stdClass() instanceof $bar) failed in %sbug70528.php on line %d

Warning: assert(): assert(new \stdClass() instanceof Bar) failed in %sbug70528.php on line %d

Warning: assert(): assert(new \stdClass() instanceof \Foo\Bar) failed in %sbug70528.php on line %d
