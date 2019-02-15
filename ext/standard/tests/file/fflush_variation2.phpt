--TEST--
Test fflush() function: usage variations - links as resource
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN')
  die("skip Links not valid on Windows");
?>
--FILE--
<?php
/*  Prototype: bool fflush ( resource $handle );
    Description: Flushes the output to a file
*/

/* test fflush() with handle to symbollic link */

$file_path = dirname(__FILE__);
require $file_path.'/file.inc';

echo "*** Testing fflush(): with soft links to files opened in diff modes ***\n";
$file_types = array("empty", "numeric", "text", "text_with_new_line", "alphanumeric");
$file_modes = array("w", "wb", "wt", "w+", "w+b", "w+t",
                    "a", "ab", "at", "a+","a+b", "a+t");

$file_name = "$file_path/fflush_variation2.tmp";
$symlink_name = "$file_path/symlnk_fflush_variation2.tmp";

$count = 1;

foreach( $file_types as $type ) {
  echo "-- Iteration $count with file containing $type data --\n";
  foreach( $file_modes as $mode ) {
    echo "-- link opened in $mode mode --\n";

    //creating the file
    $file_handle = fopen($file_name, "w");
    if($file_handle == false)
      exit("Error:failed to open file $file_name");

    //fill the file with some data if mode is append mode
    if( substr($mode, 0, 1) == "a" )
      fill_file($file_handle, $type, 10);

    //close the file
    fclose($file_handle);

    // creating the sym link
    var_dump( symlink($file_name, $symlink_name) );
    $file_handle = fopen($symlink_name, $mode);
    if($file_handle == false)
      exit("Error:failed to open link $symlink_name");

    // filling data into the file
    var_dump( fill_file($file_handle, $type, 50) );
    var_dump( fflush($file_handle) );
    fclose($file_handle);

    // reading the data from the file
    var_dump( readfile($symlink_name) );

    unlink($symlink_name);
    unlink($file_name);
  }
  $count++;
}

echo "\n*** Done ***";
?>
--EXPECT--
*** Testing fflush(): with soft links to files opened in diff modes ***
-- Iteration 1 with file containing empty data --
-- link opened in w mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in wb mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in wt mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in w+ mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in w+b mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in w+t mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in a mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in ab mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in at mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in a+ mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in a+b mode --
bool(true)
bool(true)
bool(true)
int(0)
-- link opened in a+t mode --
bool(true)
bool(true)
bool(true)
int(0)
-- Iteration 2 with file containing numeric data --
-- link opened in w mode --
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- link opened in wb mode --
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- link opened in wt mode --
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- link opened in w+ mode --
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- link opened in w+b mode --
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- link opened in w+t mode --
bool(true)
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- link opened in a mode --
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- link opened in ab mode --
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- link opened in at mode --
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- link opened in a+ mode --
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- link opened in a+b mode --
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- link opened in a+t mode --
bool(true)
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- Iteration 3 with file containing text data --
-- link opened in w mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- link opened in wb mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- link opened in wt mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- link opened in w+ mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- link opened in w+b mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- link opened in w+t mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- link opened in a mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- link opened in ab mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- link opened in at mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- link opened in a+ mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- link opened in a+b mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- link opened in a+t mode --
bool(true)
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- Iteration 4 with file containing text_with_new_line data --
-- link opened in w mode --
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- link opened in wb mode --
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- link opened in wt mode --
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- link opened in w+ mode --
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- link opened in w+b mode --
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- link opened in w+t mode --
bool(true)
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- link opened in a mode --
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- link opened in ab mode --
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- link opened in at mode --
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- link opened in a+ mode --
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- link opened in a+b mode --
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- link opened in a+t mode --
bool(true)
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- Iteration 5 with file containing alphanumeric data --
-- link opened in w mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- link opened in wb mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- link opened in wt mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- link opened in w+ mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- link opened in w+b mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- link opened in w+t mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- link opened in a mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- link opened in ab mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- link opened in at mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- link opened in a+ mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- link opened in a+b mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- link opened in a+t mode --
bool(true)
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)

*** Done ***
