--TEST--
oss-fuzz #59764: Test
--FILE--
<?php
const B = []::{A};
?>
--EXPECTF--
Fatal error: Class name must be a valid object or a string in %s on line %d
