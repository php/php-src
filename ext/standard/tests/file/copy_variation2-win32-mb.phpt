--TEST--
Test copy() function: usage variations - destination file names(special chars)
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip only run on Windows");
?>
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
   Description: Makes a copy of the file source to dest.
     Returns TRUE on success or FALSE on failure.
*/

/* Test copy() function: In creation of destination file names containing special characters
     and checking the existence and size of destination files
*/

echo "*** Test copy() function: destination file names containing special characters ***\n";
$file_path = dirname(__FILE__);
$src_file_name = $file_path."/copy_variation2私はガラスを食べられます.tmp";
$file_handle = fopen($src_file_name, "w");
fwrite( $file_handle, str_repeat("Hello2World...\n", 100) );
fclose($file_handle);

/* array of destination file names */
$dest_files = array(

  /* File names containing special(non-alpha numeric) characters */
  "_copy_variation2.tmp",
  "@copy_variation2.tmp",
  "#copy_variation2.tmp",
  "+copy_variation2.tmp",
  "?copy_variation2.tmp",
  ">copy_variation2.tmp",
  "!copy_variation2.tmp",
  "&copy_variation2.tmp",
  "(copy_variation2.tmp",
  ":copy_variation2.tmp",
  ";copy_variation2.tmp",
  "=copy_variation2.tmp",
  "[copy_variation2.tmp",
  "^copy_variation2.tmp",
  "{copy_variation2.tmp",
  "|copy_variation2.tmp",
  "~copy_variation2.tmp",
  "\$copy_variation2.tmp"
);

echo "Size of the source file before copy operation => ";
var_dump( filesize("$src_file_name") );
clearstatcache();

echo "\n--- Now applying copy() on source file to create copies ---";
$count = 1;
foreach($dest_files as $dest_file) {
  echo "\n-- Iteration $count --\n";
  $dest_file_name = $file_path."/$dest_file";

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
unlink(dirname(__FILE__)."/copy_variation2私はガラスを食べられます.tmp");
?>
--EXPECTF--
*** Test copy() function: destination file names containing special characters ***
Size of the source file before copy operation => int(1500)

--- Now applying copy() on source file to create copies ---
-- Iteration 1 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/_copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 2 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/@copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 3 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/#copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 4 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/+copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 5 --
Copy operation => 
Warning: copy(%s): %s
bool(false)
Existence of destination file => bool(false)

-- Iteration 6 --
Copy operation => 
Warning: copy(%s): %s
bool(false)
Existence of destination file => bool(false)

-- Iteration 7 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/!copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 8 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/&copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 9 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/(copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 10 --
Copy operation => 
Warning: copy(%s): %s
bool(false)
Existence of destination file => bool(false)

-- Iteration 11 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/;copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 12 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/=copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 13 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/[copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 14 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/^copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 15 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/{copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 16 --
Copy operation => 
Warning: copy(%s): %s
bool(false)
Existence of destination file => bool(false)

-- Iteration 17 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/~copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)

-- Iteration 18 --
Copy operation => bool(true)
Existence of destination file => bool(true)
Destination file name => %s/$copy_variation2.tmp
Size of source file => int(1500)
Size of destination file => int(1500)
*** Done ***
