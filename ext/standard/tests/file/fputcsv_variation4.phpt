--TEST--
Test fputcsv() : usage variations - with delimiter and enclosure as NULL
--FILE--
<?php
/*
 Prototype: array fputcsv ( resource $handle , array $fields [, string $delimiter [, string $enclosure]]] );
 Description: Format line as CSV and write to the file pointer
*/

/* Testing fputcsv() to write to a file when delimiter and enclosure is NULL */

echo "*** Testing fputcsv() : with delimiter and enclosure as NULL ***\n";

/* the array is with three elements in it. Each element should be read as
   1st element is delimiter, 2nd element is enclosure
   and 3rd element is csv fields
*/
$csv_lists = array (
  array(',', '"', array('water','fruit') ),
  array(',', '"', array('"water","fruit') ),
  array(',', '"', array('"water","fruit"') ),
  array(' ', '^', array('^water^ ^fruit^')),
  array(':', '&', array('&water&:&fruit&')),
  array('=', '=', array('=water===fruit=')),
  array('-', '-', array('-water--fruit-air')),
  array('-', '-', array('-water---fruit---air-')),
  array(':', '&', array('&""""&:&"&:,:":&,&:,,,,'))

);
$file_path = dirname(__FILE__);
$filename = "$file_path/fputcsv_variation4.tmp";

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
    var_dump( fputcsv($file_handle, $csv_field, NULL, NULL) );
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
--EXPECTF--
*** Testing fputcsv() : with delimiter and enclosure as NULL ***

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in r+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in a+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in w+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+ --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+b --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""

-- file opened in x+t --

Warning: fputcsv(): delimiter must be a character in %s on line %d
bool(false)
int(0)
bool(false)
string(0) ""
Done
