--TEST--
sha1() with ASCII output
--FILE--
<?php

$filename = dirname(__FILE__)."/sha1.dat";
$a = array(
	"abc",
	"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
	"a",
	"0123456701234567012345670123456701234567012345670123456701234567",
	""
);

foreach ($a as $str) {
	var_dump($val1 = sha1($str));
	file_put_contents($filename, $str);
	var_dump($val2 = sha1_file($filename));
	var_dump($val1 === $val2);
}

var_dump(sha1($str, true));
var_dump(sha1_file($filename, true));

@unlink($filename);

sha1_file($filename);

echo "Done\n";
?>
--EXPECTF--
unicode(40) "a9993e364706816aba3e25717850c26c9cd0d89d"
unicode(40) "a9993e364706816aba3e25717850c26c9cd0d89d"
bool(true)
unicode(40) "84983e441c3bd26ebaae4aa1f95129e5e54670f1"
unicode(40) "84983e441c3bd26ebaae4aa1f95129e5e54670f1"
bool(true)
unicode(40) "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"
unicode(40) "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"
bool(true)
unicode(40) "e0c094e867ef46c350ef54a7f59dd60bed92ae83"
unicode(40) "e0c094e867ef46c350ef54a7f59dd60bed92ae83"
bool(true)
unicode(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
unicode(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
bool(true)
string(20) "%s"
string(20) "%s"

Warning: sha1_file(%ssha1.dat): failed to open stream: No such file or directory in %s on line %d
Done
