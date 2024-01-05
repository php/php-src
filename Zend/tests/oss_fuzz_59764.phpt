--TEST--
oss-fuzz #59764: Test
--FILE--
<?php
const B = []::{A};
?>
--EXPECTF--
Fatal error: Dynamic class names are not allowed in compile-time class constant references in %s on line %d
