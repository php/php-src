--TEST--
mcrypt_list_algorithms
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
foreach (mcrypt_list_algorithms() as $algo) {
	if (in_array($algo, array('rijndael-256', 'des', 'blowfish', 'twofish'))) {
	   echo "FOUND\n";
	}
}
--EXPECT--
FOUND
FOUND
FOUND
FOUND