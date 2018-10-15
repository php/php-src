--TEST--
Test fgets() function : basic functionality
--FILE--
<?php
/*
 Prototype: string fgets ( resource $handle [, int $length] );
 Description: Gets a line from file pointer
*/

// include the file.inc for common test funcitons
include ("file.inc");

$file_modes = array("r", "rb", "rt", "r+", "r+b", "r+t");

$file_content_types = array("numeric", "text", "text_with_new_line", "alphanumeric");

echo "*** Testing fgets() : basic functionality ***\n";
foreach($file_modes as $file_mode) {
  echo "\n-- Testing fgets() with file opened using mode $file_mode --\n";
  foreach($file_content_types as $file_content_type) {
    echo "-- File content type : $file_content_type --\n";
    /* create files with $file_content_type */
    create_files ( dirname(__FILE__), 1, $file_content_type, 0755, 50, "w", "fgets_basic", 1, "bytes"); //create a file
    $filename = dirname(__FILE__)."/fgets_basic1.tmp"; // this is name of the file created by create_files()
    $file_handle = fopen($filename, $file_mode);
    if ( !$file_handle ) {
      echo "Error: failed to open file $filename!";
      exit();
    }

    echo "-- fgets() with default length, file pointer at 0 --\n";
    var_dump( fgets($file_handle) ); // with default length
    var_dump( ftell($file_handle) ); // ensure the file pointer position
    var_dump( feof($file_handle) );  // enusre if eof set

    echo "-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --\n";
    var_dump( rewind($file_handle) );
    var_dump( fgets($file_handle, 23) ); // expected: 22 chars
    var_dump( ftell($file_handle) ); // ensure the file pointer position
    var_dump( feof($file_handle) );  // enusre if eof set

    //close file
    fclose($file_handle);

    // delete file
    delete_file($filename);
  } // file_content_type loop
} // file_mode loop

echo "Done\n";
?>
--EXPECT--
*** Testing fgets() : basic functionality ***

-- Testing fgets() with file opened using mode r --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode rb --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode rt --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode r+ --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode r+b --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode r+t --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
bool(true)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)
Done
