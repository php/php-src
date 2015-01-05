--TEST--
Test fgetss() function : Basic functionality - read modes only
--FILE--
<?php
/*
 Prototype: string fgetss ( resource $handle [, int $length [, string $allowable_tags]] );
 Description: Gets line from file pointer and strip HTML tags
*/

/* test fgetss with all read modes */

// include the common file related test functions 
include ("file.inc");

echo "*** Testing fgetss() : Basic operations ***\n";

/* string with html and php tags */
$string_with_tags = <<<EOT
<test>Testing fgetss() functions</test>
<?php echo "this string is within php tag"; ?> {;}<{> this
is a heredoc string. <pg>ksklnm@@$$&$&^%&^%&^%&</pg>
<html> html </html> <?php echo "php"; ?>
EOT;

if(substr(PHP_OS, 0, 3) == "WIN")  {
	$string_with_tags = str_replace("\r",'', $string_with_tags);
}
/* try reading the file opened in different modes of reading */
$file_modes = array("r","rb", "rt","r+", "r+b", "r+t");

for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
  echo "\n-- Testing fgetss() with file opened using $file_modes[$mode_counter] mode --\n";

  /* create an empty file and write the strings with tags */
  $filename = dirname(__FILE__)."/fgetss_basic1.tmp"; 
  create_file ($filename); //create an empty file
  file_put_contents($filename, $string_with_tags); 
  $file_handle = fopen($filename, $file_modes[$mode_counter]);
  if(!$file_handle) {
    echo "Error: failed to open file $filename!\n";
    exit();
  }
  
  rewind($file_handle); 
  /* read entire file and strip tags */ 
  echo "-- fgetss() with default length, file pointer at 0 --\n";
  var_dump( fgetss($file_handle) ); // no length and allowable tags provided, reads entire file
  var_dump( ftell($file_handle) );
  var_dump( feof($file_handle) );
 
  rewind($file_handle);
  /* read entire file and strip tags tags */
  echo "-- fgets() with length = 30, file pointer at 0 --\n";
  var_dump( fgetss($file_handle ,30) ); // length parameter given,not reading entire file
  var_dump( ftell($file_handle) ); // checking file pointer position initially
  var_dump( feof($file_handle) ); // confirm file pointer is not at eof
 
  // close the file 
  fclose($file_handle);
 
  // delete the file 
  delete_file($filename);
} // end of for - mode_counter

echo "Done\n";
?>
--EXPECT--
*** Testing fgetss() : Basic operations ***

-- Testing fgetss() with file opened using r mode --
-- fgetss() with default length, file pointer at 0 --
string(27) "Testing fgetss() functions
"
int(40)
bool(false)
-- fgets() with length = 30, file pointer at 0 --
string(23) "Testing fgetss() functi"
int(29)
bool(false)

-- Testing fgetss() with file opened using rb mode --
-- fgetss() with default length, file pointer at 0 --
string(27) "Testing fgetss() functions
"
int(40)
bool(false)
-- fgets() with length = 30, file pointer at 0 --
string(23) "Testing fgetss() functi"
int(29)
bool(false)

-- Testing fgetss() with file opened using rt mode --
-- fgetss() with default length, file pointer at 0 --
string(27) "Testing fgetss() functions
"
int(40)
bool(false)
-- fgets() with length = 30, file pointer at 0 --
string(23) "Testing fgetss() functi"
int(29)
bool(false)

-- Testing fgetss() with file opened using r+ mode --
-- fgetss() with default length, file pointer at 0 --
string(27) "Testing fgetss() functions
"
int(40)
bool(false)
-- fgets() with length = 30, file pointer at 0 --
string(23) "Testing fgetss() functi"
int(29)
bool(false)

-- Testing fgetss() with file opened using r+b mode --
-- fgetss() with default length, file pointer at 0 --
string(27) "Testing fgetss() functions
"
int(40)
bool(false)
-- fgets() with length = 30, file pointer at 0 --
string(23) "Testing fgetss() functi"
int(29)
bool(false)

-- Testing fgetss() with file opened using r+t mode --
-- fgetss() with default length, file pointer at 0 --
string(27) "Testing fgetss() functions
"
int(40)
bool(false)
-- fgets() with length = 30, file pointer at 0 --
string(23) "Testing fgetss() functi"
int(29)
bool(false)
Done
