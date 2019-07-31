--TEST--
Package declaration references an unknown package
--FILE--
<?php

package "foo/bar";

?>
--EXPECTF--
Fatal error: Unknown package "foo/bar" in %s on line %d
