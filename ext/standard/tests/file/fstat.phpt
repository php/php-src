--TEST--
fstat() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/fstat.dat";

$fp = fopen($filename, "w");
var_dump(fstat($fp));
fclose($fp);
var_dump(fstat($fp));

@unlink($filename);
echo "Done\n";
?>
--EXPECTF--	
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(%d)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}

Warning: fstat(): %d is not a valid stream resource in %s on line %d
bool(false)
Done
--UEXPECTF--
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(%d)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}

Warning: fstat(): %d is not a valid stream resource in %s on line %d
bool(false)
Done
