--TEST--
Cannot take property of a string
--FILE--
<?php

"foo"->bar;

?>
--EXPECTF--
Warning: Trying to get property 'bar' of non-object in %s on line %d
