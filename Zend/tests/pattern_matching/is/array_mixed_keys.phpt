--TEST--
Array pattern with mixed implicit and explicit keys
--FILE--
<?php

var_dump([] is ['foo', 1 => 'bar']);

?>
--EXPECTF--
Fatal error: Must not mix implicit and explicit array keys in array pattern in %s on line %d
