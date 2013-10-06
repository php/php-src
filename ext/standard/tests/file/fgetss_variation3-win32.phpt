--TEST--
Test fgetss() function : usage variations - read/write modes 
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only valid for Windows');
}
?>
--FILE--
<?php
/*
 Prototype: string fgetss ( resource $handle [, int $length [, string $allowable_tags]] );
 Description: Gets line from file pointer and strip HTML tags
*/

/* try fgetss on files which are opened in read/write modes
    w+, w+b, w+t,
    a+, a+b, a+t,
    x+, x+b, x+t
 reading line by line with allowable tags: <test>, <html>, <?>
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

$filename = dirname(__FILE__)."/fgetss_variation3.tmp"; 

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
  rewind($file_handle);
  var_dump( ftell($file_handle) );
  var_dump( filesize($filename) );
  var_dump( feof($file_handle) );

  /* rewind the file and read the file  line by line with allowable tags */
  echo "-- Reading line by line with allowable tags: <test>, <html>, <?> --\n";
  $line = 1;
  while( !feof($file_handle) ) {
     echo "-- Line $line --\n"; $line++;
     var_dump( fgetss($file_handle, 80, "<test>, <html>, <?>") );
     var_dump( ftell($file_handle) );  // check the file pointer position
     var_dump( feof($file_handle) );  // check if eof reached
  }
 
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
int(0)
int(445)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using w+b mode --
int(0)
int(445)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using w+t mode --
int(0)
int(453)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using a+ mode --
int(0)
int(445)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using a+b mode --
int(0)
int(445)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using a+t mode --
int(0)
int(453)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using x+ mode --
int(0)
int(445)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using x+b mode --
int(0)
int(445)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)

-- Testing fgetss() with file opened using x+t mode --
int(0)
int(453)
bool(false)
-- Reading line by line with allowable tags: <test>, <html>, <?> --
-- Line 1 --
string(40) "<test>Testing fgetss() functions</test>
"
int(40)
bool(false)
-- Line 2 --
string(10) " {;} this
"
int(99)
bool(false)
-- Line 3 --
string(44) "is a heredoc string. ksklnm@@$$&$&^%&^%&^%&
"
int(152)
bool(false)
-- Line 4 --
string(21) "<html> html </html> 
"
int(193)
bool(false)
-- Line 5 --
string(43) "this line is without any html and php tags
"
int(236)
bool(false)
-- Line 6 --
string(79) "this is a line with more than eighty character,want to check line splitting cor"
int(315)
bool(false)
-- Line 7 --
string(27) "rectly after 80 characters
"
int(342)
bool(false)
-- Line 8 --
string(46) "this text contains some html tags  body   br 
"
int(410)
bool(false)
-- Line 9 --
string(23) "this is the line with 
"
int(433)
bool(false)
-- Line 10 --
string(12) " character. "
int(445)
bool(true)
Done
