--TEST--
Test fgetss() function : usage variations - read/write modes, file pointer at EOF
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only valid for Windows');
}
?>
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

/*
 Prototype: string fgetss ( resource $handle [, int $length [, string $allowable_tags]] );
 Description: Gets line from file pointer and strip HTML tags
*/

/* try fgetss on files which are opened in read/write modes
    w+, w+b, w+t,
    a+, a+b, a+t,
    x+, x+b, x+t
*/


echo "*** Testing fgetss() : usage variations ***\n";

/* string with html and php tags */
$string_with_tags = <<<EOT
<test>Testing fgetss() functions</test>
<?php echo "this string is within php tag"; ?> {;}<{> this
is a heredoc string. <pg>ksklnm@@$$&$&^%&^%&^%&</pg>
<html> html </html> <?php echo "php"; ?>
this line is without any html and php tags
this is a line with more than eighty character,want to check line splitting correctly after 80 characters
this text contains some html tags <body> body </body> <br> br </br>
this is the line with \n character. 
EOT;
if(substr(PHP_OS, 0, 3) == "WIN")  {
	$string_with_tags = str_replace("\r",'', $string_with_tags);
}

$filename = __DIR__."/fgetss_variation5.tmp";

/* try reading the file opened in different modes of reading */
$file_modes = array("w+","w+b", "w+t","a+", "a+b", "a+t","x+","x+b","x+t");

for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
  echo "\n-- Testing fgetss() with file opened using $file_modes[$mode_counter] mode --\n";

  /* create an empty file and write the strings with tags */
  $file_handle = fopen($filename, $file_modes[$mode_counter]);
  fwrite($file_handle,$string_with_tags); //writing data to the file
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
  unlink($filename);
} // end of for - mode_counter

echo "Done\n";
?>
--EXPECT--
*** Testing fgetss() : usage variations ***

-- Testing fgetss() with file opened using w+ mode --
int(445)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(445)
bool(false)
int(445)
bool(true)
bool(false)
int(445)
bool(true)

-- Testing fgetss() with file opened using w+b mode --
int(445)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(445)
bool(false)
int(445)
bool(true)
bool(false)
int(445)
bool(true)

-- Testing fgetss() with file opened using w+t mode --
int(453)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(453)
bool(false)
int(453)
bool(true)
bool(false)
int(453)
bool(true)

-- Testing fgetss() with file opened using a+ mode --
int(445)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(445)
bool(false)
int(445)
bool(true)
bool(false)
int(445)
bool(true)

-- Testing fgetss() with file opened using a+b mode --
int(445)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(445)
bool(false)
int(445)
bool(true)
bool(false)
int(445)
bool(true)

-- Testing fgetss() with file opened using a+t mode --
int(453)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(453)
bool(false)
int(453)
bool(true)
bool(false)
int(453)
bool(true)

-- Testing fgetss() with file opened using x+ mode --
int(445)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(445)
bool(false)
int(445)
bool(true)
bool(false)
int(445)
bool(true)

-- Testing fgetss() with file opened using x+b mode --
int(445)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(445)
bool(false)
int(445)
bool(true)
bool(false)
int(445)
bool(true)

-- Testing fgetss() with file opened using x+t mode --
int(453)
bool(true)
int(0)
bool(false)
-- Reading when file pointer points to EOF --
int(0)
int(453)
bool(false)
int(453)
bool(true)
bool(false)
int(453)
bool(true)
Done
