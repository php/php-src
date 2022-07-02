--TEST--
method_exists() on internal classes
--FILE--
<?php
echo " ---(Internal classes, using string class name)---\n";
echo "Does exception::getmessage exist? ";
var_dump(method_exists("exception", "getmessage"));
echo "Does stdclass::nonexistent exist? ";
var_dump(method_exists("stdclass", "nonexistent"));

echo "\n ---(Internal classes, using class instance)---\n";
echo "Does exception::getmessage exist? ";
var_dump(method_exists(new exception, "getmessage"));
echo "Does stdclass::nonexistent exist? ";
var_dump(method_exists(new stdclass, "nonexistent"));

echo "Done";
?>
--EXPECT--
 ---(Internal classes, using string class name)---
Does exception::getmessage exist? bool(true)
Does stdclass::nonexistent exist? bool(false)

 ---(Internal classes, using class instance)---
Does exception::getmessage exist? bool(true)
Does stdclass::nonexistent exist? bool(false)
Done
