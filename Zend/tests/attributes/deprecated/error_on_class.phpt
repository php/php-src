--TEST--
#[\Deprecated]: Using on a class
--FILE--
<?php

#[\Deprecated]
class Demo {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\Deprecated] to class Demo in %s on line %d
