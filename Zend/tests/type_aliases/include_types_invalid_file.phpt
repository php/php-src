--TEST--
include types with invalid file (contains non-type-alias code)
--FILE--
<?php
include types 'invalid_types.php';
?>
--EXPECTF--
Fatal error: include types: Types file 'invalid_types.php' must only contain type aliases (use type ... as ...;) in %s on line %d
