--TEST--
Test copy() function: usage variations - copy empty file across dirs
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only run on Windows");
?>
--FILE--
<?php
/* Test copy() function: Trying to create copy of source file
     into different destination dir paths given in various notations */

echo "*** Test copy() function: copying file across directories ***\n";
$base_dir = __DIR__."/copy_variation6";
mkdir($base_dir);

$sub_dir = $base_dir."/copy_variation6_sub";
mkdir($sub_dir);

$dirname_with_blank = $sub_dir."/copy variation6";
mkdir($dirname_with_blank);

$src_file_name = __DIR__."/copy_variation6.tmp";
fclose( fopen($src_file_name, "w") );

echo "Size of source file => ";
var_dump( filesize($src_file_name) );
clearstatcache();

$dests = array(
  $base_dir."/copy_copy_variation6.tmp",
  $base_dir."/copy_variation6_sub/copy_copy_variation6.tmp",
  "$sub_dir/copy_copy_variation6.tmp",
  "$sub_dir/../copy_copy_variation6.tmp",
  "$sub_dir/../copy_variation6_sub/copy_copy_variation6.tmp",
  "$sub_dir/..///../copy_copy_variation6.tmp",
  "$sub_dir///../*",
  "$dirname_with_blank/copy_copy_variation6.tmp"
);

echo "\n-- Now applying copy() on source file to create copies --";
$count = 1;
foreach($dests as $dest) {
  echo "\n-- Iteration $count --\n";

  echo "Copy operation => ";
  var_dump( copy($src_file_name, $dest) );

  echo "Existence of destination file => ";
  var_dump( file_exists($dest) );

   if( file_exists($dest) ) {
    echo "Destination file name is => ";
    print($dest);
    echo "\n";

    echo "Size of source file => ";
    var_dump( filesize($src_file_name) );
    clearstatcache();

    echo "Size of destination file => ";
    var_dump( filesize($dest) );
    clearstatcache();

    unlink("$dest");
  }

  $count++;
}

unlink($src_file_name);
rmdir($dirname_with_blank);
rmdir($sub_dir);
rmdir($base_dir);

echo "*** Done ***\n";
?>
--EXPECTF--
*** Test copy() function: copying file across directories ***
Size of source file => int(0)

-- Now applying copy() on source file to create copies --
-- Iteration 1 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name is => %s/copy_variation6/copy_copy_variation6.tmp
Size of source file => int(0)
Size of destination file => int(0)

-- Iteration 2 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name is => %s/copy_variation6/copy_variation6_sub/copy_copy_variation6.tmp
Size of source file => int(0)
Size of destination file => int(0)

-- Iteration 3 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name is => %s/copy_variation6/copy_variation6_sub/copy_copy_variation6.tmp
Size of source file => int(0)
Size of destination file => int(0)

-- Iteration 4 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name is => %s/copy_variation6/copy_variation6_sub/../copy_copy_variation6.tmp
Size of source file => int(0)
Size of destination file => int(0)

-- Iteration 5 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name is => %s/copy_variation6/copy_variation6_sub/../copy_variation6_sub/copy_copy_variation6.tmp
Size of source file => int(0)
Size of destination file => int(0)

-- Iteration 6 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name is => %s/copy_variation6/copy_variation6_sub/..///../copy_copy_variation6.tmp
Size of source file => int(0)
Size of destination file => int(0)

-- Iteration 7 --
Copy operation => 
Warning: copy(%s/copy_variation6/copy_variation6_sub///../*): Failed to open stream: No such file or directory in %s on line %d
bool(false)
Existence of destination file => bool(false)

-- Iteration 8 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name is => %s/copy_variation6/copy_variation6_sub/copy variation6/copy_copy_variation6.tmp
Size of source file => int(0)
Size of destination file => int(0)
*** Done ***
