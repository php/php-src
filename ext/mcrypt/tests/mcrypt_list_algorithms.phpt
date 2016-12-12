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
--EXPECTF--
Deprecated: Function mcrypt_list_algorithms() is deprecated in %s%emcrypt_list_algorithms.php on line 2
FOUND
FOUND
FOUND
FOUND