--TEST--
Closure: Binding + RT cache edge cases
--FILE--
<?php

// cache_size may be zero
$f = function () {};
$f();
$g = $f->bindTo(new class {});

?>
==DONE==
--EXPECT--
==DONE==
