--TEST--
Callable forbidden as class name since it is also a type hint
--FILE--
<?php
// Previously this was a thrown ParseError - now it's a fatal compilation error.
class Callable {
}
?>
--EXPECTF--
Fatal error: Cannot use 'Callable' as class name as it is reserved in %s on line 3