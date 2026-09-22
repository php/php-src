--TEST--
GH-15976: Bad interface names
--FILE--
<?php

interface _ {}
interface bool {}

?>
--EXPECTF--
Deprecated: Using "_" as an interface name is deprecated since 8.4 in %sinterface-names.php on line 3

Fatal error: Cannot use "bool" as an interface name as it is reserved in %sinterface-names.php on line 4
