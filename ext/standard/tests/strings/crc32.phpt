--TEST--
crc32() function
--POST--
--GET--
--FILE--
<?php
$input = array("foo", "bar", "baz", "grldsajkopallkjasd");
foreach($input AS $i) {
	echo crc32($i)."\n";
}
?>
--EXPECT--
-1938594527
1996459178
2015626392
824412087
