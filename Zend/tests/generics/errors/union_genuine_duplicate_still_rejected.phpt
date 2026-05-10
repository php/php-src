--TEST--
Generics: a genuine non-generic duplicate in a union is still rejected
--FILE--
<?php
class Box<T> {}
class Other {}
function f(): Box<int>|Other|Other {}
?>
--EXPECTF--
Fatal error: Duplicate type Other is redundant in %s on line %d
