--TEST--
Cannot take property of a string
--FILE--
<?php

"foo"->bar;

?>
--EXPECTF--
Notice: Trying to get property of non-object in %s on line %d
