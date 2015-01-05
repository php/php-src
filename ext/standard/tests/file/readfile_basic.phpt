--TEST--
Test readfile() function: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only on Linux');
}
?>
--FILE--
<?php
/* Prototype: int readfile ( string $filename [, bool $use_include_path [, resource $context]] );
   Description: Outputs a file
*/
// common file used
require(dirname(__FILE__) . '/file.inc');

echo "*** Testing readfile() : basic functionality ***\n";
$file_path = dirname(__FILE__);
$file_prefix = "readfile_basic";  // temp files created with this prefix

// the content that is filled into the temp files as created
$filetypes = array("numeric", "text", "empty", "alphanumeric", "text_with_new_line");
// different file modes
$filemodes = array("w", "wt", "wb", "w+", "w+b", "w+t",
                   "a", "at", "ab", "a+", "a+b", "a+t",
                   "x", "xb", "xt", "x+", "x+b", "x+t");

// create file, read the file content, delete file
foreach($filetypes as $type) {
  echo "\n-- File filled with content type: $type --\n";
  foreach($filemodes as $mode) {
    echo "-- File opened with mode: $mode --\n";
      if ( strstr($mode, "x") ) {
         $fp = fopen($file_path."/".$file_prefix."1.tmp", $mode);
         fill_file($fp, $type, 100);
         fclose($fp);
      } else {
        // creating file in write mode
        create_files($file_path, 1, $type, 0755, 100, $mode, $file_prefix, 1, "byte");
      }
      $count = readfile($file_path."/".$file_prefix."1.tmp");
      echo "\n";
      var_dump($count);
      // delete files created
      delete_files($file_path, 1, $file_prefix, 1);
  }
}
echo "Done\n";
?>
--EXPECT--
*** Testing readfile() : basic functionality ***

-- File filled with content type: numeric --
-- File opened with mode: w --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: wt --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: wb --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: w+ --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: w+b --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: w+t --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: a --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: at --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: ab --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: a+ --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: a+b --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: a+t --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: x --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: xb --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: xt --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: x+ --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: x+b --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)
-- File opened with mode: x+t --
2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
int(100)

-- File filled with content type: text --
-- File opened with mode: w --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: wt --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: wb --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: w+ --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: w+b --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: w+t --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: a --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: at --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: ab --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: a+ --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: a+b --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: a+t --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: x --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: xb --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: xt --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: x+ --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: x+b --
text text text text text text text text text text text text text text text text text text text text 
int(100)
-- File opened with mode: x+t --
text text text text text text text text text text text text text text text text text text text text 
int(100)

-- File filled with content type: empty --
-- File opened with mode: w --

int(0)
-- File opened with mode: wt --

int(0)
-- File opened with mode: wb --

int(0)
-- File opened with mode: w+ --

int(0)
-- File opened with mode: w+b --

int(0)
-- File opened with mode: w+t --

int(0)
-- File opened with mode: a --

int(0)
-- File opened with mode: at --

int(0)
-- File opened with mode: ab --

int(0)
-- File opened with mode: a+ --

int(0)
-- File opened with mode: a+b --

int(0)
-- File opened with mode: a+t --

int(0)
-- File opened with mode: x --

int(0)
-- File opened with mode: xb --

int(0)
-- File opened with mode: xt --

int(0)
-- File opened with mode: x+ --

int(0)
-- File opened with mode: x+b --

int(0)
-- File opened with mode: x+t --

int(0)

-- File filled with content type: alphanumeric --
-- File opened with mode: w --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: wt --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: wb --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: w+ --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: w+b --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: w+t --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: a --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: at --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: ab --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: a+ --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: a+b --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: a+t --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: x --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: xb --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: xt --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: x+ --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: x+b --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)
-- File opened with mode: x+t --
ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 
int(100)

-- File filled with content type: text_with_new_line --
-- File opened with mode: w --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: wt --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: wb --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: w+ --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: w+b --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: w+t --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: a --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: at --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: ab --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: a+ --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: a+b --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: a+t --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: x --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: xb --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: xt --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: x+ --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: x+b --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
-- File opened with mode: x+t --
line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line 
int(100)
Done
