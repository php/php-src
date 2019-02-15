--TEST--
SplFileObject::fputcsv(): Usage variations -- with line without any CSV fields
--FILE--
<?php

/* Testing fputcsv() to write to a file when the field has no CSV format */

echo "*** Testing fputcsv() : with no CSV format in the field ***\n";

/* the array is with three elements in it. Each element should be read as
   1st element is delimiter, 2nd element is enclosure
   and 3rd element is csv fields
*/

$fields = array( array('water_fruit\n'),
                array("water_fruit\n"),
                array("")
         );

$file_path = dirname(__FILE__);
$file = "$file_path/fputcsv_variation10.tmp";

$file_modes = array ("r+", "r+b", "r+t",
                     "a+", "a+b", "a+t",
                     "w+", "w+b", "w+t",
                     "x+", "x+b", "x+t");

$loop_counter = 1;
foreach ($fields as $field) {
  for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {

    echo "\n-- file opened in $file_modes[$mode_counter] --\n";
    // create the file and add the content with has csv fields
    if ( strstr($file_modes[$mode_counter], "r") ) {
      $fo = new SplFileObject($file, 'w');
    } else {
      $fo = new SplFileObject($file, $file_modes[$mode_counter]);
    }
    $csv_field = $field;

    // write to a file in csv format
    var_dump( $fo->fputcsv($csv_field) );

    // check the file pointer position and eof
    var_dump( $fo->ftell() );
    var_dump( $fo->eof() );
    //close the file
    unset($fo);

    // print the file contents
    var_dump( file_get_contents($file) );

    //delete file
    unlink($file);
  } //end of mode loop
} // end of foreach

echo "Done\n";
?>
--EXPECTF--
*** Testing fputcsv() : with no CSV format in the field ***

-- file opened in r+ --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in r+b --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in r+t --
int(16)
int(16)
bool(false)
string(%d) ""water_fruit\n"
"

-- file opened in a+ --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in a+b --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in a+t --
int(16)
int(16)
bool(false)
string(%d) ""water_fruit\n"
"

-- file opened in w+ --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in w+b --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in w+t --
int(16)
int(16)
bool(false)
string(%d) ""water_fruit\n"
"

-- file opened in x+ --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in x+b --
int(16)
int(16)
bool(false)
string(16) ""water_fruit\n"
"

-- file opened in x+t --
int(16)
int(16)
bool(false)
string(%d) ""water_fruit\n"
"

-- file opened in r+ --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in r+b --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in r+t --
int(15)
int(15)
bool(false)
string(%d) ""water_fruit
"
"

-- file opened in a+ --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in a+b --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in a+t --
int(15)
int(15)
bool(false)
string(%d) ""water_fruit
"
"

-- file opened in w+ --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in w+b --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in w+t --
int(15)
int(15)
bool(false)
string(%d) ""water_fruit
"
"

-- file opened in x+ --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in x+b --
int(15)
int(15)
bool(false)
string(15) ""water_fruit
"
"

-- file opened in x+t --
int(15)
int(15)
bool(false)
string(%d) ""water_fruit
"
"

-- file opened in r+ --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in r+b --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in r+t --
int(1)
int(1)
bool(false)
string(%d) "
"

-- file opened in a+ --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in a+b --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in a+t --
int(1)
int(1)
bool(false)
string(%d) "
"

-- file opened in w+ --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in w+b --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in w+t --
int(1)
int(1)
bool(false)
string(%d) "
"

-- file opened in x+ --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in x+b --
int(1)
int(1)
bool(false)
string(1) "
"

-- file opened in x+t --
int(1)
int(1)
bool(false)
string(%d) "
"
Done
