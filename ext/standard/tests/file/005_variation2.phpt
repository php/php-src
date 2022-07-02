--TEST--
Test fileatime(), filemtime(), filectime() & touch() functions : usage variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Non Windows Systems');
}
?>
--FILE--
<?php

function stat_fn( $filename ) {
  echo "\n-- File '$filename' --\n";
  echo "-- File access time is => ";
  echo fileatime($filename)."\n";
  clearstatcache();
  echo "-- File modification time is => ";
  echo filemtime($filename)."\n";
  clearstatcache();
  echo "-- inode change time is => ";
  echo filectime($filename)."\n";
  clearstatcache();


}

echo "*** Testing fileattime(), filemtime(), filectime() & touch() : usage variations ***\n";
echo "\n*** testing touch ***\n";
$b = touch(false);
$c = touch('');
$d = touch(' ');
$e = touch('|');

var_dump($a);
var_dump($b);
var_dump($c);
var_dump($d);
var_dump($e);

echo "\n*** testing file info ***";
stat_fn(false);
stat_fn('');
stat_fn(' ');
stat_fn('|');

var_dump(unlink(' '));
var_dump(unlink('|'));

echo "Done";
?>
--EXPECTF--
*** Testing fileattime(), filemtime(), filectime() & touch() : usage variations ***

*** testing touch ***

Warning: Undefined variable $a in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(true)

*** testing file info ***
-- File '' --
-- File access time is => 
-- File modification time is => 
-- inode change time is => 

-- File '' --
-- File access time is => 
-- File modification time is => 
-- inode change time is => 

-- File ' ' --
-- File access time is => %d
-- File modification time is => %d
-- inode change time is => %d

-- File '|' --
-- File access time is => %d
-- File modification time is => %d
-- inode change time is => %d
bool(true)
bool(true)
Done
