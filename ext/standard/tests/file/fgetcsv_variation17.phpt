--TEST--
Test fgetcsv() : usage variations - with default enclosure & length less than line size
--FILE--
<?php
/* 
 Prototype: array fgetcsv ( resource $handle [, int $length [, string $delimiter [, string $enclosure]]] );
 Description: Gets line from file pointer and parse for CSV fields
*/

/* Testing fgetcsv() to read a file when provided with default enclosure character
   and length value less than the size of line being read
 */

echo "*** Testing fgetcsv() : with default enclosure & length less than line size ***\n";

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

$filename = dirname(__FILE__) . '/fgetcsv_variation17.tmp';
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
    // working when only a blan line is read
    fwrite($file_handle, "This is line of text without csv fields\n");
    fwrite($file_handle, "\n"); // blank line

    // close the file if the mode to be used is read mode  and re-open using read mode
    // else rewind the file pointer to begining of the file 
    if ( strstr($file_modes[$mode_counter], "r" ) ) {
      fclose($file_handle);
      $file_handle = fopen($filename, $file_modes[$mode_counter]);
    } else {
      // rewind the file pointer to bof
      rewind($file_handle);
    }
      
    echo "\n-- Testing fgetcsv() with file opened using $file_modes[$mode_counter] mode --\n"; 

    // call fgetcsv() to parse csv fields

    // use length as less than the actual size of the line 
    fseek($file_handle, 0, SEEK_SET);
    var_dump( fgetcsv($file_handle, 9, $delimiter) );
    // check the file pointer position and if eof
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );

    // read rest of the line 
    var_dump( fgetcsv($file_handle, 1024, $delimiter) );
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
--EXPECT--
*** Testing fgetcsv() : with default enclosure & length less than line size ***

-- Testing fgetcsv() with file opened using r mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(3) "fru"
}
int(9)
bool(false)
array(1) {
  [0]=>
  string(2) "it"
}
int(12)
bool(false)

-- Testing fgetcsv() with file opened using r mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --
array(2) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
}
int(16)
bool(false)
array(8) {
  [0]=>
  string(4) "This"
  [1]=>
  string(2) "is"
  [2]=>
  string(4) "line"
  [3]=>
  string(2) "of"
  [4]=>
  string(4) "text"
  [5]=>
  string(7) "without"
  [6]=>
  string(3) "csv"
  [7]=>
  string(6) "fields"
}
int(56)
bool(false)

-- Testing fgetcsv() with file opened using r mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(3) "air"
}
int(20)
bool(false)
array(1) {
  [0]=>
  string(39) "This is line of text without csv fields"
}
int(60)
bool(false)

-- Testing fgetcsv() with file opened using r mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using rb mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using rt mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using r+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using r+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using r+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using a+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using a+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using a+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using w+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using w+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using w+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using x+ mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using x+b mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)

-- Testing fgetcsv() with file opened using x+t mode --
array(3) {
  [0]=>
  string(5) "water"
  [1]=>
  string(5) "fruit"
  [2]=>
  string(43) ""
This is line of text without csv fields

"
}
int(61)
bool(true)
bool(false)
int(61)
bool(true)
Done
