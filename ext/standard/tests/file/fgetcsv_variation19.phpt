--TEST--
Test fgetcsv() : usage variations - with default enclosure & delimiter of two chars
--FILE--
<?php
/*
 Prototype: array fgetcsv ( resource $handle [, int $length [, string $delimiter [, string $enclosure]]] );
 Description: Gets line from file pointer and parse for CSV fields
*/

/*
   Testing fgetcsv() to read a file when provided with default enclosure character
   and with delimiter of two characters
*/

echo "*** Testing fgetcsv() : with default enclosure & delimiter of two chars ***\n";

/* the array is with two elements in it. Each element should be read as
   1st element is delimiter & 2nd element is csv fields
*/
$csv_lists = array (
  array(',', 'water,fruit'),
  array(' ', 'water fruit'),
  array(' ', '"water" "fruit"'),
  array('\\', 'water\\"fruit"\\"air"'),
  array('\\', '"water"\\"fruit"\\"""'),
);

$filename = __DIR__ . '/fgetcsv_variation19.tmp';
@unlink($filename);

$file_modes = array ("r","rb", "rt", "r+", "r+b", "r+t",
                     "a+", "a+b", "a+t",
                     "w+", "w+b", "w+t",
                     "x+", "x+b", "x+t");

$loop_counter = 1;
foreach ($csv_lists as $csv_list) {
  for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
    // create the file and add the content with has csv fields
    if ( strstr($file_modes[$mode_counter], "r") ) {
      $file_handle = fopen($filename, "w");
    } else {
      $file_handle = fopen($filename, $file_modes[$mode_counter] );
    }
    if ( !$file_handle ) {
      echo "Error: failed to create file $filename!\n";
      exit();
    }
    $delimiter = $csv_list[0];
    $csv_field = $csv_list[1];
    fwrite($file_handle, $csv_field . "\n");
    // write another line of text and a blank line
    // this will be used to test, if the fgetcsv() read more than a line and its
    // working when only a blank line is read
    fwrite($file_handle, "This is line of text without csv fields\n");
    fwrite($file_handle, "\n"); // blank line

    // close the file if the mode to be used is read mode  and re-open using read mode
    // else rewind the file pointer to beginning of the file
    if ( strstr($file_modes[$mode_counter], "r" ) ) {
      fclose($file_handle);
      $file_handle = fopen($filename, $file_modes[$mode_counter]);
    } else {
      // rewind the file pointer to bof
      rewind($file_handle);
    }

    echo "\n-- Testing fgetcsv() with file opened using $file_modes[$mode_counter] mode --\n";

    // call fgetcsv() to parse csv fields

    // use delimiter & enclosure char of two chars
    fseek($file_handle, 0, SEEK_SET);
    $del = "++";
    var_dump( fgetcsv($file_handle, 1024, $del) );
    // check the file pointer position and if eof
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );

    // close the file
    fclose($file_handle);
    //delete file
    unlink($filename);
  } //end of mode loop
} // end of foreach

echo "Done\n";
?>
--EXPECTF--
*** Testing fgetcsv() : with default enclosure & delimiter of two chars ***

-- Testing fgetcsv() with file opened using r mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water,fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(11) "water fruit"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(13) "water "fruit""
}
int(16)
bool(false)

-- Testing fgetcsv() with file opened using r mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(19) "water\"fruit"\"air""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using r mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --

Notice: fgetcsv(): delimiter must be a single character in %s on line %d
array(1) {
  [0]=>
  string(17) "water\"fruit"\""""
}
int(20)
bool(false)
Done
