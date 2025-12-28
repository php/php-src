--TEST--
GH-15976: Bad class names
--FILE--
<?php

class _ {}
class bool {}

?>
--EXPECTF--
Deprecated: Using "_" as a class name is deprecated since 8.4 in %sclass-names.php on line 3

Fatal error: Cannot use "bool" as a class name as it is reserved in %sclass-names.php on line 4
