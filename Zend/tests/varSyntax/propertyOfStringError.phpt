--TEST--
Cannot take property of a string
--FILE--
<?php

"foo"->bar;

?>
--EXPECTF--
Warning: Attempt to read property "bar" on string in %s on line %d
