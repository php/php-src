--TEST--
GH-15976: Bad class alias names
--FILE--
<?php

class_alias( 'stdClass', '_' );
class_alias( 'stdClass', 'bool' );

?>
--EXPECTF--
Deprecated: Using "_" as a class name is deprecated since 8.4 in %salias-names.php on line 3

Fatal error: Cannot use 'bool' as class name as it is reserved in %salias-names.php on line 4
