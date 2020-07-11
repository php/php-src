--TEST--
Test fgets() function : usage variations - read with/without length
--FILE--
<?php
// include the file.inc for common test functions
include ("file.inc");

$file_modes = array("w+", "w+b", "w+t",
                    "a+", "a+b", "a+t",
                    "x+", "x+b", "x+t");

$file_content_types = array("numeric", "text", "text_with_new_line", "alphanumeric");

echo "*** Testing fgets() : usage variations ***\n";

$filename = __DIR__."/fgets_variation3.tmp";

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

    echo "-- fgets() with default length, file pointer at 0 --\n";
    // get the file pointer to beginning of the file
    rewind($file_handle);

    var_dump( ftell($file_handle) );
    var_dump( fgets($file_handle) ); // with default length
    var_dump( ftell($file_handle) ); // ensure the file pointer position
    var_dump( feof($file_handle) );  // ensure if eof set

    echo "-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --\n";
    // get the file pointer to beginning of the file
    rewind($file_handle);

    var_dump( ftell($file_handle) );
    var_dump( fgets($file_handle, 23) ); // expected: 22 chars
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
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode w+b --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode w+t --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode a+ --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode a+b --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode a+t --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode x+ --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode x+b --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)

-- Testing fgets() with file opened using mode x+t --
-- File content type : numeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "22222222222222222222222222222222222222222222222222"
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "2222222222222222222222"
int(22)
bool(false)
-- File content type : text --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "text text text text text text text text text text "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "text text text text te"
int(22)
bool(false)
-- File content type : text_with_new_line --
-- fgets() with default length, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(5) "line
"
int(5)
bool(false)
-- File content type : alphanumeric --
-- fgets() with default length, file pointer at 0 --
int(0)
string(50) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
int(50)
bool(true)
-- fgets() with length = 23, Expected: 22 chars, file pointer at 0 --
int(0)
string(22) "ab12 ab12 ab12 ab12 ab"
int(22)
bool(false)
Done
