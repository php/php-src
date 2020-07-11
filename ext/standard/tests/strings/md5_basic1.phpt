--TEST--
Test md5() function : basic functionality
--FILE--
<?php
echo "*** Testing md5() : basic functionality ***\n";
var_dump(md5("apple"));
?>
--EXPECT--
*** Testing md5() : basic functionality ***
string(32) "1f3870be274f6c49b3e31a0c6728957f"
