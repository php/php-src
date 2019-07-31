--TEST--
Package declaration must be the first statement (even before declares)
--FILE--
<?php

declare(strict_types=1);
package "foo/bar";

?>
--EXPECTF--
Fatal error: Package declaration must be the first statement in the script in %s on line %d
