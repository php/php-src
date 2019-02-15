--TEST--
Test fputcsv() : usage variations - two chars as enclosure & delimiter
--FILE--
<?php
/*
 Prototype: array fputcsv ( resource $handle , array $fields [, string $delimiter [, string $enclosure]]] );
 Description: Format line as CSV and write to the file pointer
*/


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
$file_path = dirname(__FILE__);
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
    var_dump( fputcsv($file_handle, $csv_field, '++', '%%'
) );
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
*** Testing fputcsv() : with two chars as enclosure & delimiter ***

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in r+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in r+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in r+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in a+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in a+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in a+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in w+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in w+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in w+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in x+ --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in x+b --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in x+t --

Notice: fputcsv(): delimiter must be a single character in %s on line %d

Notice: fputcsv(): enclosure must be a single character in %s on line %d
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"
Done
