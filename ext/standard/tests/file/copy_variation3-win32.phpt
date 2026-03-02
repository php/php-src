--TEST--
Test copy() function: usage variations - destination file names(white spaces)
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip only run on Windows");
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
/* Test copy() function: In creation of destination file names containing white spaces
     and checking the existence and size of destination files
*/

echo "*** Test copy() function: destination file names containing whitespaces ***\n";
$file_path = __DIR__;
$src_file_name = $file_path."/copy_variation3.tmp";
$file_handle = fopen($src_file_name, "w");
fwrite( $file_handle, str_repeat("Hello2World...\n", 100) );
fclose($file_handle);

/* array of destination file names */
$dest_files = array(

  /* File names containing whitespaces */
  "copy variation3.tmp",  //file name containing blank space
  " copy_variation3.tmp",  //file name starts with blank space
  "copy\tvariation3.tmp",
  " ",  //blank space as file name
);

echo "Size of the source file before copy operation => ";
var_dump( filesize("$src_file_name") );
clearstatcache();

echo "\n-- Now applying copy() on source file to create copies --";
$count = 1;
foreach($dest_files as $dest_file) {

  echo "\n-- Iteration $count --\n";
  $dest_file_name = $dest_file;

  echo "Copy operation => ";
  var_dump( copy($src_file_name, $dest_file_name) );

  echo "Existence of destination file => ";
  var_dump( file_exists($dest_file_name) );

  if( file_exists($dest_file_name) ) {
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
  }
  $count++;
}

echo "*** Done ***\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/copy_variation3.tmp");
?>
--EXPECTF--
*** Test copy() function: destination file names containing whitespaces ***
Size of the source file before copy operation => int(1500)

-- Now applying copy() on source file to create copies --
-- Iteration 1 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => copy variation3.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 2 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name =>  copy_variation3.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 3 --
Copy operation => 
Warning: copy(%s): %s
bool(false)
Existence of destination file => bool(false)

-- Iteration 4 --
Copy operation => 
Warning: copy(%s): %s
bool(false)
Existence of destination file => bool(false)
*** Done ***
