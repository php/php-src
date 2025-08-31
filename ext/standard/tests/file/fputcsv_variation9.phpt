--TEST--
Test fputcsv() : usage variations - two chars as enclosure & delimiter
--FILE--
<?php
/* Testing fputcsv() to write to a file when delimiter and enclosure are of two chars each */

echo "*** Testing fputcsv() : with two chars as enclosure & delimiter ***\n";

/* the array is with three elements in it. Each element should be read as
   1st element is delimiter, 2nd element is enclosure
   and 3rd element is csv fields
*/
$csv_lists = array (
  array(',', '"', array('water,fruit') ),
  array(',', '"', array('"water","fruit') ),
  array(',', '"', array('"water","fruit"') ),
  array(' ', '^', array('^water^ ^fruit^')),
  array(':', '&', array('&water&:&fruit&')),
  array('=', '=', array('=water===fruit=')),
  array('-', '-', array('-water--fruit-air')),
  array('-', '-', array('-water---fruit---air-')),
  array(':', '&', array('&""""&:&"&:,:":&,&:,,,,'))

);
$file_path = __DIR__;
$filename = "$file_path/fputcsv_variation9.tmp";

$file_modes = array ("r+", "r+b", "r+t",
                     "a+", "a+b", "a+t",
                     "w+", "w+b", "w+t",
                     "x+", "x+b", "x+t");

$loop_counter = 1;
foreach ($csv_lists as $csv_list) {
  for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {

    echo "\n-- file opened in $file_modes[$mode_counter] --\n";
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
    $enclosure = $csv_list[1];
    $csv_field = $csv_list[2];

    // write to a file in csv format
    try {
      var_dump( fputcsv($file_handle, $csv_field, '++', '%%') );
    } catch (ValueError $e) {
      echo $e->getMessage(), "\n";
    }
    // check the file pointer position and eof
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );
    //close the file
    fclose($file_handle);

    // print the file contents
    var_dump( file_get_contents($filename) );

    //delete file
    unlink($filename);
  } //end of mode loop
} // end of foreach

echo "Done\n";
?>
--EXPECT--
*** Testing fputcsv() : with two chars as enclosure & delimiter ***

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in r+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in a+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in w+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+ --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+b --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""

-- file opened in x+t --
fputcsv(): Argument #3 ($separator) must be a single character
int(0)
bool(false)
string(0) ""
Done
