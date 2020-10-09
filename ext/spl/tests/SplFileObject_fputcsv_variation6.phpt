--TEST--
Test fputcsv() : usage variations - with different delimiter and enclosure
--FILE--
<?php

/*
   Testing fputcsv() to write to a file when delimiter are different from those
   present in the field to be written to the file
 */

echo "*** Testing fputcsv() : with different delimiter and enclosure ***\n";

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
$file = "$file_path/fputcsv_variation6.tmp";

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
      $fo = new SplFileObject($file, 'w');
    } else {
      $fo = new SplFileObject($file, $file_modes[$mode_counter]);
    }
    $delimiter = $csv_list[0];
    $enclosure = $csv_list[1];
    $csv_field = $csv_list[2];

    // write to a file in csv format
    var_dump( $fo->fputcsv($csv_field, '+', '%') );
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
*** Testing fputcsv() : with different delimiter and enclosure ***

-- file opened in r+ --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in r+b --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in r+t --
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in a+ --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in a+b --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in a+t --
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in w+ --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in w+b --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in w+t --
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in x+ --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in x+b --
int(12)
int(12)
bool(false)
string(12) "water,fruit
"

-- file opened in x+t --
int(12)
int(12)
bool(false)
string(%d) "water,fruit
"

-- file opened in r+ --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in r+b --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in r+t --
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in a+ --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in a+b --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in a+t --
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in w+ --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in w+b --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in w+t --
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in x+ --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in x+b --
int(15)
int(15)
bool(false)
string(15) ""water","fruit
"

-- file opened in x+t --
int(15)
int(15)
bool(false)
string(%d) ""water","fruit
"

-- file opened in r+ --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in r+b --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in r+t --
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in a+ --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in a+b --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in a+t --
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in w+ --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in w+b --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in w+t --
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in x+ --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in x+b --
int(16)
int(16)
bool(false)
string(16) ""water","fruit"
"

-- file opened in x+t --
int(16)
int(16)
bool(false)
string(%d) ""water","fruit"
"

-- file opened in r+ --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in r+b --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in r+t --
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in a+ --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in a+b --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in a+t --
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in w+ --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in w+b --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in w+t --
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in x+ --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in x+b --
int(18)
int(18)
bool(false)
string(18) "%^water^ ^fruit^%
"

-- file opened in x+t --
int(18)
int(18)
bool(false)
string(%d) "%^water^ ^fruit^%
"

-- file opened in r+ --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in r+b --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in r+t --
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in a+ --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in a+b --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in a+t --
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in w+ --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in w+b --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in w+t --
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in x+ --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in x+b --
int(16)
int(16)
bool(false)
string(16) "&water&:&fruit&
"

-- file opened in x+t --
int(16)
int(16)
bool(false)
string(%d) "&water&:&fruit&
"

-- file opened in r+ --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in r+b --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in r+t --
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in a+ --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in a+b --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in a+t --
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in w+ --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in w+b --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in w+t --
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in x+ --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in x+b --
int(16)
int(16)
bool(false)
string(16) "=water===fruit=
"

-- file opened in x+t --
int(16)
int(16)
bool(false)
string(%d) "=water===fruit=
"

-- file opened in r+ --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in r+b --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in r+t --
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in a+ --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in a+b --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in a+t --
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in w+ --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in w+b --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in w+t --
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in x+ --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in x+b --
int(18)
int(18)
bool(false)
string(18) "-water--fruit-air
"

-- file opened in x+t --
int(18)
int(18)
bool(false)
string(%d) "-water--fruit-air
"

-- file opened in r+ --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in r+b --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in r+t --
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in a+ --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in a+b --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in a+t --
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in w+ --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in w+b --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in w+t --
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in x+ --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in x+b --
int(22)
int(22)
bool(false)
string(22) "-water---fruit---air-
"

-- file opened in x+t --
int(22)
int(22)
bool(false)
string(%d) "-water---fruit---air-
"

-- file opened in r+ --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in r+b --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in r+t --
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in a+ --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in a+b --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in a+t --
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in w+ --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in w+b --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in w+t --
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in x+ --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in x+b --
int(24)
int(24)
bool(false)
string(24) "&""""&:&"&:,:":&,&:,,,,
"

-- file opened in x+t --
int(24)
int(24)
bool(false)
string(%d) "&""""&:&"&:,:":&,&:,,,,
"
Done
