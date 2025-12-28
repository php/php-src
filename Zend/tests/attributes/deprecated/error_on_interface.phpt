--TEST--
#[\Deprecated]: Using on an interface
--FILE--
<?php

#[\Deprecated]
interface Demo {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\Deprecated] to interface Demo in %s on line %d
