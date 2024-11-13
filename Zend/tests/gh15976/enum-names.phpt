--TEST--
GH-15976: Bad enum names
--FILE--
<?php

enum _ {}
enum bool {}

?>
--EXPECTF--
Deprecated: Using "_" as an enum name is deprecated since 8.4 in %senum-names.php on line 3

Fatal error: Cannot use "bool" as an enum name as it is reserved in %senum-names.php on line 4
