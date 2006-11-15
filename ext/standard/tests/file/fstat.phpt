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
  int(%i)
  [1]=>
  int(%i)
  [2]=>
  int(%i)
  [3]=>
  int(%i)
  [4]=>
  int(%i)
  [5]=>
  int(%i)
  [6]=>
  int(%i)
  [7]=>
  int(%i)
  [8]=>
  int(%i)
  [9]=>
  int(%i)
  [10]=>
  int(%i)
  [11]=>
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
  ["ino"]=>
  int(%i)
  ["mode"]=>
  int(%i)
  ["nlink"]=>
  int(%i)
  ["uid"]=>
  int(%i)
  ["gid"]=>
  int(%i)
  ["rdev"]=>
  int(%i)
  ["size"]=>
  int(%i)
  ["atime"]=>
  int(%i)
  ["mtime"]=>
  int(%i)
  ["ctime"]=>
  int(%i)
  ["blksize"]=>
  int(%i)
  ["blocks"]=>
  int(%i)
}

Warning: fstat(): %d is not a valid stream resource in %s on line %d
bool(false)
Done
