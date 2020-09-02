--TEST--
Test getmypid() function: basic test
--FILE--
<?php
echo "Simple testcase for getmypid() function\n";

var_dump(getmypid());

echo "Done\n";
?>
--EXPECTF--
Simple testcase for getmypid() function
int(%d)
Done
