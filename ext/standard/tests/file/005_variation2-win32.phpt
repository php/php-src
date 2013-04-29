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
/*
   Prototype: int fileatime ( string $filename );
   Description: Returns the time the file was last accessed, or FALSE 
     in case of an error. The time is returned as a Unix timestamp.

   Prototype: int filemtime ( string $filename );
   Description: Returns the time the file was last modified, or FALSE 
     in case of an error. 

   Prototype: int filectime ( string $filename );
   Description: Returns the time the file was last changed, or FALSE
     in case of an error. The time is returned as a Unix timestamp.

   Prototype: bool touch ( string $filename [, int $time [, int $atime]] );
   Description: Attempts to set the access and modification times of the file
     named in the filename parameter to the value given in time.
*/

/*
   Prototype: void stat_fn(string $filename);
   Description: Prints access, modification and change times of a file
*/
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
echo "\n*** testing file info ***";
stat_fn(NULL);
stat_fn(false);
stat_fn('');
stat_fn(' ');
stat_fn('|');
echo "\n*** testing touch ***";
var_dump(touch(NULL));
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
-- File access time is => 
-- File modification time is => 
-- inode change time is => 

-- File '' --
-- File access time is => 
-- File modification time is => 
-- inode change time is => 

-- File '' --
-- File access time is => 
-- File modification time is => 
-- inode change time is => 

-- File ' ' --
-- File access time is => 
Warning: fileatime(): stat failed for   in %s on line %d

-- File modification time is => 
Warning: filemtime(): stat failed for   in %s on line %d

-- inode change time is => 
Warning: filectime(): stat failed for   in %s on line %d


-- File '|' --
-- File access time is => 
Warning: fileatime(): stat failed for | in %s on line %d

-- File modification time is => 
Warning: filemtime(): stat failed for | in %s on line %d

-- inode change time is => 
Warning: filectime(): stat failed for | in %s on line %d


*** testing touch ***bool(false)
bool(false)
bool(false)

Warning: touch(): %s in %s on line %d
bool(false)

Warning: touch(): %s in %s on line %d
bool(false)
Done
