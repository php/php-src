--TEST--
Test fgetss() function : usage variations - write only modes
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only on Windows');
}
?>
--FILE--
<?php
/*
 Prototype: string fgetss ( resource $handle [, int $length [, string $allowable_tags]] );
 Description: Gets line from file pointer and strip HTML tags
*/

/* try fgets on files which are opened in non readable modes
    w, wb, wt,
    a, ab, at,
    x, xb, xt
*/

// include the common file related test functions
include ("file.inc");

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

$filename = dirname(__FILE__)."/fgetss_variation1.tmp";

/* try reading the file opened in different modes of reading */
$file_modes = array("w","wb", "wt","a", "ab", "at","x","xb","xt");

for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
  echo "\n-- Testing fgetss() with file opened using $file_modes[$mode_counter] mode --\n";

  /* create an empty file and write the strings with tags */
  $file_handle = fopen($filename, $file_modes[$mode_counter]);
  fwrite($file_handle,$string_with_tags);
  if(!$file_handle) {
    echo "Error: failed to open file $filename!\n";
    exit();
  }

  // rewind the file pointer to beginning of the file
  var_dump( filesize($filename) );
  var_dump( rewind($file_handle) );
  var_dump( ftell($file_handle) );
  var_dump( feof($file_handle) );

  /* read entire file and strip tags */
  echo "-- fgetss() with default length, file pointer at 0 , expected : no character should be read --\n";
  var_dump( fgetss($file_handle) ); // expected : no character should be read
  var_dump( ftell($file_handle) ); //ensure that file pointer position is not changed
  var_dump( feof($file_handle) ); // check if end of file pointer is set

  // close the file
  fclose($file_handle);

  // delete the file
  delete_file($filename);
} // end of for - mode_counter

echo "Done\n";
?>
--EXPECT--
*** Testing fgetss() : usage variations ***

-- Testing fgetss() with file opened using w mode --
int(445)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using wb mode --
int(445)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using wt mode --
int(453)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using a mode --
int(445)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using ab mode --
int(445)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using at mode --
int(453)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using x mode --
int(445)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using xb mode --
int(445)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)

-- Testing fgetss() with file opened using xt mode --
int(453)
bool(true)
int(0)
bool(false)
-- fgetss() with default length, file pointer at 0 , expected : no character should be read --
bool(false)
int(0)
bool(false)
Done
