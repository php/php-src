--TEST--
Bug #79783 (segfault in str_replace)
--FILE--
<?php

str_replace( "", "", "", defined(1) );
echo "fail";
--EXPECTF--
Fatal error: Uncaught TypeError: str_replace(): Argument #4 ($replace_count) must be a reference, bool given in %s/bug79783.php:3
Stack trace:
#0 %s/bug79783.php(3): str_replace('', '', '', false)
#1 {main}
  thrown in %s/bug79783.php on line 3
