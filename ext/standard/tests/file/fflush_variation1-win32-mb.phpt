--TEST--
Test fflush() function: usage variations - files in different modes
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die("skip.. only for Windows");
?>
--FILE--
<?php
/*  Prototype: bool fflush ( resource $handle );
    Description: Flushes the output to a file
*/

/* test fflush() with handle to the files opened in different modes */

$file_path = dirname(__FILE__);
require $file_path.'/file.inc';

echo "*** Testing fflush(): with various types of files ***\n";
$file_types = array("empty", "numeric", "text", "text_with_new_line", "alphanumeric");
$file_modes = array("w", "wb", "wt", "w+", "w+b", "w+t",
                    "a", "ab", "at", "a+","a+b", "a+t",
                    "x", "xb", "xt", "x+", "x+b", "x+t");

$file_name = "$file_path/fflush_variation私はガラスを食べられます1.tmp";

$count = 1;

foreach( $file_types as $type ) {
  echo "-- Iteration $count with file containing $type Data--\n";
  foreach( $file_modes as $mode ) {
    echo "-- File opened in $mode mode --\n";

    // creating the file except for x mode
    if( substr($mode, 0, 1) != "x" ) {
      $file_handle = fopen($file_name, "w");
      if($file_handle == false)
        exit("Error:failed to open file $file_name");

      // filling the file some data if mode is append mode
      if( substr($mode, 0, 1) == "a")
        fill_file($file_handle, $type, 10);
      fclose($file_handle);
    }

    // opening the file in different modes
    $file_handle = fopen($file_name, $mode);
    if($file_handle == false)
      exit("Error:failed to open file $file_name");

    // writing data to the file
    var_dump( fill_file($file_handle, $type, 50) );
    var_dump( fflush($file_handle) );
    fclose($file_handle);

    // reading the contents of the file after flushing
     var_dump( readfile($file_name) );
     unlink($file_name);
  }
  $count++;
}

echo "\n*** Done ***";
?>
--EXPECTF--
*** Testing fflush(): with various types of files ***
-- Iteration 1 with file containing empty Data--
-- File opened in w mode --
bool(true)
bool(true)
int(0)
-- File opened in wb mode --
bool(true)
bool(true)
int(0)
-- File opened in wt mode --
bool(true)
bool(true)
int(0)
-- File opened in w+ mode --
bool(true)
bool(true)
int(0)
-- File opened in w+b mode --
bool(true)
bool(true)
int(0)
-- File opened in w+t mode --
bool(true)
bool(true)
int(0)
-- File opened in a mode --
bool(true)
bool(true)
int(0)
-- File opened in ab mode --
bool(true)
bool(true)
int(0)
-- File opened in at mode --
bool(true)
bool(true)
int(0)
-- File opened in a+ mode --
bool(true)
bool(true)
int(0)
-- File opened in a+b mode --
bool(true)
bool(true)
int(0)
-- File opened in a+t mode --
bool(true)
bool(true)
int(0)
-- File opened in x mode --
bool(true)
bool(true)
int(0)
-- File opened in xb mode --
bool(true)
bool(true)
int(0)
-- File opened in xt mode --
bool(true)
bool(true)
int(0)
-- File opened in x+ mode --
bool(true)
bool(true)
int(0)
-- File opened in x+b mode --
bool(true)
bool(true)
int(0)
-- File opened in x+t mode --
bool(true)
bool(true)
int(0)
-- Iteration 2 with file containing numeric Data--
-- File opened in w mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in wb mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in wt mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in w+ mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in w+b mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in w+t mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in a mode --
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- File opened in ab mode --
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- File opened in at mode --
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- File opened in a+ mode --
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- File opened in a+b mode --
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- File opened in a+t mode --
bool(true)
bool(true)
222222222222222222222222222222222222222222222222222222222222int(60)
-- File opened in x mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in xb mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in xt mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in x+ mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in x+b mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- File opened in x+t mode --
bool(true)
bool(true)
22222222222222222222222222222222222222222222222222int(50)
-- Iteration 3 with file containing text Data--
-- File opened in w mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in wb mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in wt mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in w+ mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in w+b mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in w+t mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in a mode --
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- File opened in ab mode --
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- File opened in at mode --
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- File opened in a+ mode --
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- File opened in a+b mode --
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- File opened in a+t mode --
bool(true)
bool(true)
text text text text text text text text text text text text int(60)
-- File opened in x mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in xb mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in xt mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in x+ mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in x+b mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- File opened in x+t mode --
bool(true)
bool(true)
text text text text text text text text text text int(50)
-- Iteration 4 with file containing text_with_new_line Data--
-- File opened in w mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in wb mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in wt mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(55)
-- File opened in w+ mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in w+b mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in w+t mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(55)
-- File opened in a mode --
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- File opened in ab mode --
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- File opened in at mode --
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(65)
-- File opened in a+ mode --
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- File opened in a+b mode --
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(60)
-- File opened in a+t mode --
bool(true)
bool(true)
line
line line
line of text
line
line of text
line
line of tint(65)
-- File opened in x mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in xb mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in xt mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(55)
-- File opened in x+ mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in x+b mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(50)
-- File opened in x+t mode --
bool(true)
bool(true)
line
line of text
line
line of text
line
line of tint(55)
-- Iteration 5 with file containing alphanumeric Data--
-- File opened in w mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in wb mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in wt mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in w+ mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in w+b mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in w+t mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in a mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- File opened in ab mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- File opened in at mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- File opened in a+ mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- File opened in a+b mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- File opened in a+t mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(60)
-- File opened in x mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in xb mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in xt mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in x+ mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in x+b mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)
-- File opened in x+t mode --
bool(true)
bool(true)
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 int(50)

*** Done ***
