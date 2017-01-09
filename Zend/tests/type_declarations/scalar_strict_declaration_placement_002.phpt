--TEST--
Test strict declaration being first operation only 002
--FILE--
<?php

namespace Foo;

declare(strict_types=1);

var_dump(strlen("abc"));

?>
--EXPECTF--
Fatal error: strict_types declaration must be the very first statement in the script in %s on line %d
