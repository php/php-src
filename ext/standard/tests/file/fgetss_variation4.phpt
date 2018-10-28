--TEST--
Test fgetss() function : usage variations - read  modes, file pointer at EOF
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip not for Windows");
?>
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

/*
 Prototype: string fgetss ( resource $handle [, int $length [, string $allowable_tags]] );
 Description: Gets line from file pointer and strip HTML tags
*/

// include the common file related test functions
include ("file.inc");

echo "*** Testing fgetss() : usage variations  ***\n";

/* string with html and php tags */
$string_with_tags = <<<EOT
<test>Testing fgetss() functions</test>
<?php echo "this string is within php tag"; ?> {;}<{> this
is a heredoc string. <pg>ksklnm@@$$&$&^%&^%&^%&</pg>
<html> html </html> <?php echo "php"; ?>
this line is without any html and php tags
this is a line with more than eighty character,want to check line splitting correctly after 80 characters
this is the text containing \r character 
this text contains some html tags <body> body </body> <br> br </br>
this is the line with \n character. 
EOT;

$filename = dirname(__FILE__)."/fgetss_variation4.tmp";

/* try reading the file opened in different modes of reading */
$file_modes = array("r","rb", "rt","r+", "r+b", "r+t");

for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
  echo "\n-- Testing fgetss() with file opened using $file_modes[$mode_counter] mode --\n";

  /* create an empty file and write the strings with tags */
  create_file ($filename); //create an empty file
  file_put_contents($filename, $string_with_tags);
  $file_handle = fopen($filename, $file_modes[$mode_counter]);
  if(!$file_handle) {
    echo "Error: failed to open file $filename!\n";
    exit();
  }

  // rewind the file pointer to beginning of the file
  var_dump( filesize($filename) );
  var_dump( rewind($file_handle) );
  var_dump( ftell($file_handle) );
  var_dump( feof($file_handle) );

  echo "-- Reading when file pointer points to EOF --\n";
  var_dump( fseek($file_handle,0,SEEK_END) ); // now file pointer at end
  var_dump( ftell($file_handle) ); //ensure file pointer at end
  var_dump( fgetss($file_handle) ); // try to read
  var_dump( ftell($file_handle) ); // find out file position
  var_dump( feof($file_handle) );  // ensure that file pointer is at eof

  // now file is at the end try reading with length and allowable tags,expecting false
  var_dump( fgetss($file_handle, 80, "<test>, <html>, <?>") );
  var_dump( ftell($file_handle) );  // find out file position
  var_dump( feof($file_handle) );   // ensure that file pointer is at eof

  // close the file
  fclose($file_handle);
  // delete the file
  delete_file($filename);
} // end of for - mode_counter

echo "Done\n";
?>
--EXPECT--
*** Testing fgetss() : usage variations  ***

-- Testing fgetss() with file opened using r mode --
int(486)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(486)
bool(false)
int(486)
bool(true)
bool(false)
int(486)
bool(true)

-- Testing fgetss() with file opened using rb mode --
int(486)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(486)
bool(false)
int(486)
bool(true)
bool(false)
int(486)
bool(true)

-- Testing fgetss() with file opened using rt mode --
int(486)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(486)
bool(false)
int(486)
bool(true)
bool(false)
int(486)
bool(true)

-- Testing fgetss() with file opened using r+ mode --
int(486)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(486)
bool(false)
int(486)
bool(true)
bool(false)
int(486)
bool(true)

-- Testing fgetss() with file opened using r+b mode --
int(486)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(486)
bool(false)
int(486)
bool(true)
bool(false)
int(486)
bool(true)

-- Testing fgetss() with file opened using r+t mode --
int(486)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(486)
bool(false)
int(486)
bool(true)
bool(false)
int(486)
bool(true)
Done
