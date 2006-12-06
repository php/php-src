--TEST--
crc32() function
--FILE--
<?php
$input = array(b"foo", b"bar", b"baz", b"grldsajkopallkjasd");
foreach($input AS $i) {
	printf("%u\n", crc32($i));
}
?>
--EXPECT--
2356372769
1996459178
2015626392
824412087
