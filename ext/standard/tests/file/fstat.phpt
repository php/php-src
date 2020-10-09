--TEST--
fstat() tests
--FILE--
<?php

$filename = __DIR__."/fstat.dat";

$fp = fopen($filename, "w");
var_dump(fstat($fp));
fclose($fp);
try {
    var_dump(fstat($fp));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

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
fstat(): supplied resource is not a valid stream resource
Done
