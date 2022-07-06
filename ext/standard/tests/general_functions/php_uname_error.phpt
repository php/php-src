--TEST--
Test php_uname() function -  error conditions - pass function incorrect arguments
--FILE--
<?php

echo "*** Testing php_uname() - error test\n";

echo "\n-- Testing php_uname() function with invalid mode --\n";
// am invalid mode should result in same o/p as mode 'a'
var_dump( php_uname('z') == php_uname('z') );

?>
--EXPECT--
*** Testing php_uname() - error test

-- Testing php_uname() function with invalid mode --
bool(true)
