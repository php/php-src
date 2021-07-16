--TEST--
Test fileatime(), filemtime(), filectime() & touch() functions : usage variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php

function stat_fn( $filename ) {
  echo "\n-- File '$filename' --\n";
  $result = fileatime($filename);
  echo "-- File access time is => ";
  echo $result."\n";
  clearstatcache();
  $result = filemtime($filename);
  echo "-- File modification time is => ";
  echo $result."\n";
  clearstatcache();
  $result = filectime($filename);
  echo "-- inode change time is => ";
  echo $result."\n";
  clearstatcache();


}

echo "*** Testing fileattime(), filemtime(), filectime() & touch() : usage variations ***\n";
echo "\n*** testing file info ***";
stat_fn(false);
stat_fn('');
stat_fn(' ');
stat_fn('|');
echo "\n*** testing touch ***\n";
var_dump(touch(false));
var_dump(touch(''));

//php generates permission denied, we generate No such file or dir.
var_dump(touch(' '));
var_dump(touch('|'));


echo "Done";
?>
--EXPECTF--
*** Testing fileattime(), filemtime(), filectime() & touch() : usage variations ***

*** testing file info ***
-- File '' --

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
-- File access time is => 

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
-- File modification time is => 

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
-- inode change time is => 

-- File '' --
-- File access time is => 
-- File modification time is => 
-- inode change time is => 

-- File ' ' --

Warning: fileatime(): stat failed for   in %s on line %d
-- File access time is => 

Warning: filemtime(): stat failed for   in %s on line %d
-- File modification time is => 

Warning: filectime(): stat failed for   in %s on line %d
-- inode change time is => 

-- File '|' --

Warning: fileatime(): stat failed for | in %s on line %d
-- File access time is => 

Warning: filemtime(): stat failed for | in %s on line %d
-- File modification time is => 

Warning: filectime(): stat failed for | in %s on line %d
-- inode change time is => 

*** testing touch ***

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
bool(false)
bool(false)

Warning: touch(): %s in %s on line %d
bool(false)

Warning: touch(): %s in %s on line %d
bool(false)
Done
