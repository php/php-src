--TEST--
#[\Deprecated]: Using on an enum
--FILE--
<?php

#[\Deprecated]
enum Demo {}

?>
--EXPECTF--
Fatal error: Cannot apply #[\Deprecated] to enum Demo in %s on line %d
