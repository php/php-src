--TEST--
GH-15976: Bad trait names
--FILE--
<?php

trait _ {}
trait bool {}

?>
--EXPECTF--
Deprecated: Using "_" as a trait name is deprecated since 8.4 in %strait-names.php on line 3

Fatal error: Cannot use "bool" as a trait name as it is reserved in %strait-names.php on line 4
