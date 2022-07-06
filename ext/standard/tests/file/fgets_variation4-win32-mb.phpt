--TEST--
Test fgets() function : usage variations - seek n read
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only valid for Windows');
}
?>
--FILE--
<?php
// include the file.inc for common test functions
include ("file.inc");

$file_modes = array("w+", "w+b", "w+t",
                    "a+", "a+b", "a+t",
                    "x+", "x+b", "x+t");

$file_content_types = array("numeric", "text", "text_with_new_line", "alphanumeric");

echo "*** Testing fgets() : usage variations ***\n";

$filename = __DIR__."/fgets_variation4私はガラスを食べられます.tmp";

foreach($file_modes as $file_mode) {
  echo "\n-- Testing fgets() with file opened using mode $file_mode --\n";

  foreach($file_content_types as $file_content_type) {
    echo "-- File content type : $file_content_type --\n";

    /* create files with $file_content_type */
    $file_handle = fopen($filename, $file_mode);
    $data = fill_file($file_handle, $file_content_type, 50);

    if ( !$file_handle ) {
      echo "Error: failed to open file $filename!";
      exit();
    }

    echo "-- fgets() with location set by fseek() with default length --\n";
    var_dump( fseek($file_handle, 5, SEEK_SET) );
    var_dump( ftell($file_handle) );
    var_dump( fgets($file_handle ) );
    var_dump( ftell($file_handle) ); // ensure the file pointer position
    var_dump( feof($file_handle) );  // ensure if eof set

    echo "-- fgets() with location set by fseek() with length = 20 --\n";
    var_dump( fseek($file_handle, 25, SEEK_SET) );
    var_dump( ftell($file_handle) );
    var_dump( fgets($file_handle, 20 ) ); // expected 19 chars
    var_dump( ftell($file_handle) ); // ensure the file pointer position
    var_dump( feof($file_handle) );  // ensure if eof set

    //close file
    fclose($file_handle);

    // delete file
    delete_file($filename);
  } // file_content_type loop
} // file_mode loop

echo "Done\n";
?>
--EXPECT--
*** Testing fgets() : usage variations ***

-- Testing fgets() with file opened using mode w+ --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(13) "line of text
"
int(18)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(11) "ne of text
"
int(36)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode w+b --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(13) "line of text
"
int(18)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(11) "ne of text
"
int(36)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode w+t --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(1) "
"
int(6)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(12) "ine of text
"
int(37)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode a+ --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(13) "line of text
"
int(18)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(11) "ne of text
"
int(36)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode a+b --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(13) "line of text
"
int(18)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(11) "ne of text
"
int(36)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode a+t --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(1) "
"
int(6)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(12) "ine of text
"
int(37)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode x+ --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(13) "line of text
"
int(18)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(11) "ne of text
"
int(36)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode x+b --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(13) "line of text
"
int(18)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(11) "ne of text
"
int(36)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)

-- Testing fgets() with file opened using mode x+t --
-- File content type : numeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "2222222222222222222"
int(44)
bool(false)
-- File content type : text --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "text text text text text text text text text "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "text text text text"
int(44)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(1) "
"
int(6)
bool(false)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(12) "ine of text
"
int(37)
bool(false)
-- File content type : alphanumeric --
-- fgets() with location set by fseek() with default length --
int(0)
int(5)
string(45) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with location set by fseek() with length = 20 --
int(0)
int(25)
string(19) "ab12 ab12 ab12 ab12"
int(44)
bool(false)
Done
