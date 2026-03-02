--TEST--
Test chr() function : basic functionality
--FILE--
<?php

echo "*** Testing chr() : basic functionality ***\n";

echo chr(72). chr(101) . chr(108) . chr(108). chr(111); // Hello
echo chr(10); // "\n"
echo "World";
echo "\n";
?>
--EXPECT--
*** Testing chr() : basic functionality ***
Hello
World
