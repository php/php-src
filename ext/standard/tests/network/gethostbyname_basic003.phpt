--TEST--
Test gethostbyname() function : basic functionality
--FILE--
<?php
echo "*** Testing gethostbyname() : basic functionality ***\n";

echo gethostbyname("localhost")."\n";
?>
--EXPECTREGEX--
\*\*\* Testing gethostbyname\(\) : basic functionality \*\*\*
(127.0.\d.1|::1)
