--TEST--
Test strict declaration being first operation only 005
--FILE--
<?php

declare(strict_types=1);

namespace Foo;

var_dump(strlen("abc"));

?>
--EXPECTF--
int(3)