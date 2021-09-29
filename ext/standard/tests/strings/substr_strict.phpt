--TEST--
Testing substr() with strict types
--FILE--
<?php

declare(strict_types=1);

echo substr('foo', 1, false);
--EXPECT--
