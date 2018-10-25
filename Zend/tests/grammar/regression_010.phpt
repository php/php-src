--TEST--
Test to check regressions on T_IMPLEMENTS followed by a T_NS_SEPARATOR
--FILE--
<?php

interface A{}

class B implements\A {}

echo "Done", PHP_EOL;
--EXPECT--
Done
