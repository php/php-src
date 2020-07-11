--TEST--
Test copy() function: usage variations - destination file names(empty string, nulls & bools)
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Do not run on Windows");

if(substr(PHP_OS, 0, 3) == "AIX")
  die("skip Do not run on AIX");
?>
--FILE--
<?php
/* Test copy() function: In creation of destination file names with empty string, nulls & bools
     and checking the existence and size of destination files
*/

echo "*** Test copy() function: destination file names with empty string, nulls & bools ***\n";
$file_path = __DIR__;
$src_file_name = $file_path."/copy_variation4.tmp";
$file_handle = fopen($src_file_name, "w");
fwrite( $file_handle, str_repeat("Hello2World...\n", 100) );
fclose($file_handle);

/* array of destination file names */
$dest_files = array(

  /* File names containing(or with) nulls */
  "",
  NULL,
  FALSE,
  false,
  TRUE,
  true
);

echo "Size of the source file before copy operation => ";
var_dump( filesize($src_file_name) );
clearstatcache();

echo "\n-- Now applying copy() on source file to create copies --";
$count = 1;
foreach($dest_files as $dest_file) {
  echo "\n-- Iteration $count --\n";
  $dest_file_name = $file_path."/$dest_file";

  echo "Existence of destination file before copy => ";
  var_dump( file_exists($dest_file_name) );

  echo "Copy operation => ";
  var_dump( copy($src_file_name, $dest_file_name) );

  echo "Existence of destination file => ";
  var_dump( file_exists($dest_file_name) );

  echo "Destination file name => ";
  print($dest_file_name);
  echo "\n";

  echo "Size of source file => ";
  var_dump( filesize($src_file_name) );
  clearstatcache();

  echo "Size of destination file => ";
  var_dump( filesize($dest_file_name) );
  clearstatcache();

  unlink($dest_file_name);

  $count++;

}

echo "*** Done ***\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/copy_variation4.tmp");
?>
--EXPECTF--
*** Test copy() function: destination file names with empty string, nulls & bools ***
Size of the source file before copy operation => int(1500)

-- Now applying copy() on source file to create copies --
-- Iteration 1 --
Existence of destination file before copy => bool(true)
Copy operation => 
Warning: copy(): The second argument to copy() function cannot be a directory in %s on line %d
bool(false)
Existence of destination file => bool(true)
Destination file name => %s/
Size of source file => int(1500)
Size of destination file => int(%d)

Warning: unlink(%s): %s

-- Iteration 2 --
Existence of destination file before copy => bool(true)
Copy operation => 
Warning: copy(): The second argument to copy() function cannot be a directory in %s on line %d
bool(false)
Existence of destination file => bool(true)
Destination file name => %s/
Size of source file => int(1500)
Size of destination file => int(%d)

Warning: unlink(%s): %s

-- Iteration 3 --
Existence of destination file before copy => bool(true)
Copy operation => 
Warning: copy(): The second argument to copy() function cannot be a directory in %s on line %d
bool(false)
Existence of destination file => bool(true)
Destination file name => %s/
Size of source file => int(1500)
Size of destination file => int(%d)

Warning: unlink(%s): %s

-- Iteration 4 --
Existence of destination file before copy => bool(true)
Copy operation => 
Warning: copy(): The second argument to copy() function cannot be a directory in %s on line %d
bool(false)
Existence of destination file => bool(true)
Destination file name => %s/
Size of source file => int(1500)
Size of destination file => int(%d)

Warning: unlink(%s): %s

-- Iteration 5 --
Existence of destination file before copy => bool(false)
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/1
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 6 --
Existence of destination file before copy => bool(false)
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/1
Size of source file => int(1500)
Size of destination file => int(1500)
*** Done ***
