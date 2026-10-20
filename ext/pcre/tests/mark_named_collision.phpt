--TEST--
preg_match: (*MARK:name) directive does not collide with named group called MARK
--FILE--
<?php
preg_match('/(?P<MARK>[abc])(*MARK:value)/', "abc", $m);
echo "count: ", count($m), "\n";
echo "MARK: ", $m['MARK'], "\n";
echo "json: ", json_encode($m), "\n";
echo "keys: ", implode(',', array_keys($m)), "\n";
?>
--EXPECT--
count: 3
MARK: value
json: {"0":"a","MARK":"value","1":"a"}
keys: 0,MARK,1
