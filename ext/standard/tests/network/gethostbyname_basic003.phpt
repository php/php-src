--TEST--
Test gethostbyname() function : basic functionality
--FILE--
<?php
echo "*** Testing gethostbyname() : basic functionality ***\n";

echo gethostbyname("localhost")."\n";
?>
--EXPECT--
*** Testing gethostbyname() : basic functionality ***
127.0.0.1
