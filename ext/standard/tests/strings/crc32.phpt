--TEST--
crc32() function
--POST--
--GET--
--FILE--
<?php
$input = array("foo", "bar", "baz", "grldsajkopallkjasd");
foreach($input AS $i) {
	printf("%u\n", crc32($i));
}
?>
--EXPECT--
2356372769
1996459178
2015626392
824412087
