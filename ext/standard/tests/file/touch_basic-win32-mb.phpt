--TEST--
Test touch() function : basic functionality
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
/* Prototype  : proto bool touch(string filename [, int time [, int atime]])
 * Description: Set modification time of file
 * Source code: ext/standard/filestat.c
 * Alias to functions:
 */

echo "*** Testing touch() : basic functionality ***\n";

$filename = dirname(__FILE__)."/touch_私はガラスを食べられます.dat";

echo "\n--- testing touch creates a file ---\n";
@unlink($filename);
if (file_exists($filename)) {
   die("touch_basic failed");
}
var_dump( touch($filename) );
if (file_exists($filename) == false) {
   die("touch_basic failed");
}

echo "\n --- testing touch doesn't alter file contents ---\n";
$testln = "Here is a test line";
$h = fopen($filename, "wb");
fwrite($h, $testln);
fclose($h);
touch($filename);
$h = fopen($filename, "rb");
echo fgets($h);
fclose($h);

echo "\n\n --- testing touch alters the correct file metadata ---\n";
$init_meta = stat($filename);
clearstatcache();
sleep(1);
touch($filename);
$next_meta = stat($filename);
$type = array("dev", "ino", "mode", "nlink", "uid", "gid",
              "rdev", "size", "atime", "mtime", "ctime",
              "blksize", "blocks");

for ($i = 0; $i < count($type); $i++) {
   if ($init_meta[$i] != $next_meta[$i]) {
      echo "stat data differs at $type[$i]\n";
   }
}


// Initialise all required variables
$time = 10000;
$atime = 20470;

// Calling touch() with all possible arguments
echo "\n --- testing touch using all parameters ---\n";
var_dump( touch($filename, $time, $atime) );
clearstatcache();
$init_meta = stat($filename);
echo "ctime=".$init_meta['ctime']."\n";
echo "mtime=".$init_meta['mtime']."\n";
echo "atime=".$init_meta['atime']."\n";

unlink($filename);

echo "Done";
?>
--EXPECTF--
*** Testing touch() : basic functionality ***

--- testing touch creates a file ---
bool(true)

 --- testing touch doesn't alter file contents ---
Here is a test line

 --- testing touch alters the correct file metadata ---
stat data differs at atime
stat data differs at mtime

 --- testing touch using all parameters ---
bool(true)
ctime=%d
mtime=10000
atime=20470
Done
