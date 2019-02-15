--TEST--
Test fputcsv() : usage variations - with same delimiter and different enclosure
--FILE--
<?php
/*
 Prototype: array fputcsv ( resource $handle , array $fields [, string $delimiter [, string $enclosure]]] );
 Description: Format line as CSV and write to the file pointer
*/

/*
   Testing fputcsv() to write to a file when delimiter is same but enclosure is different from those
   present in the field to be written to the file
 */

echo "*** Testing fputcsv() : with same delimiter and different enclosure ***\n";

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
$filename = "$file_path/fputcsv_variation8.tmp";

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
    var_dump( fputcsv($file_handle, $csv_field, $delimiter, '+') );
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
*** Testing fputcsv() : with same delimiter and different enclosure ***

-- file opened in r+ --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in r+b --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in r+t --
int(14)
int(14)
bool(false)
string(%d) "+water,fruit+
"

-- file opened in a+ --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in a+b --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in a+t --
int(14)
int(14)
bool(false)
string(%d) "+water,fruit+
"

-- file opened in w+ --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in w+b --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in w+t --
int(14)
int(14)
bool(false)
string(%d) "+water,fruit+
"

-- file opened in x+ --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in x+b --
int(14)
int(14)
bool(false)
string(14) "+water,fruit+
"

-- file opened in x+t --
int(14)
int(14)
bool(false)
string(%d) "+water,fruit+
"

-- file opened in r+ --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in r+b --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in r+t --
int(17)
int(17)
bool(false)
string(%d) "+"water","fruit+
"

-- file opened in a+ --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in a+b --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in a+t --
int(17)
int(17)
bool(false)
string(%d) "+"water","fruit+
"

-- file opened in w+ --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in w+b --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in w+t --
int(17)
int(17)
bool(false)
string(%d) "+"water","fruit+
"

-- file opened in x+ --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in x+b --
int(17)
int(17)
bool(false)
string(17) "+"water","fruit+
"

-- file opened in x+t --
int(17)
int(17)
bool(false)
string(%d) "+"water","fruit+
"

-- file opened in r+ --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in r+b --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in r+t --
int(18)
int(18)
bool(false)
string(%d) "+"water","fruit"+
"

-- file opened in a+ --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in a+b --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in a+t --
int(18)
int(18)
bool(false)
string(%d) "+"water","fruit"+
"

-- file opened in w+ --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in w+b --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in w+t --
int(18)
int(18)
bool(false)
string(%d) "+"water","fruit"+
"

-- file opened in x+ --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in x+b --
int(18)
int(18)
bool(false)
string(18) "+"water","fruit"+
"

-- file opened in x+t --
int(18)
int(18)
bool(false)
string(%d) "+"water","fruit"+
"

-- file opened in r+ --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in r+b --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in r+t --
int(18)
int(18)
bool(false)
string(%d) "+^water^ ^fruit^+
"

-- file opened in a+ --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in a+b --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in a+t --
int(18)
int(18)
bool(false)
string(%d) "+^water^ ^fruit^+
"

-- file opened in w+ --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in w+b --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in w+t --
int(18)
int(18)
bool(false)
string(%d) "+^water^ ^fruit^+
"

-- file opened in x+ --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in x+b --
int(18)
int(18)
bool(false)
string(18) "+^water^ ^fruit^+
"

-- file opened in x+t --
int(18)
int(18)
bool(false)
string(%d) "+^water^ ^fruit^+
"

-- file opened in r+ --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in r+b --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in r+t --
int(18)
int(18)
bool(false)
string(%d) "+&water&:&fruit&+
"

-- file opened in a+ --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in a+b --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in a+t --
int(18)
int(18)
bool(false)
string(%d) "+&water&:&fruit&+
"

-- file opened in w+ --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in w+b --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in w+t --
int(18)
int(18)
bool(false)
string(%d) "+&water&:&fruit&+
"

-- file opened in x+ --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in x+b --
int(18)
int(18)
bool(false)
string(18) "+&water&:&fruit&+
"

-- file opened in x+t --
int(18)
int(18)
bool(false)
string(%d) "+&water&:&fruit&+
"

-- file opened in r+ --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in r+b --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in r+t --
int(18)
int(18)
bool(false)
string(%d) "+=water===fruit=+
"

-- file opened in a+ --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in a+b --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in a+t --
int(18)
int(18)
bool(false)
string(%d) "+=water===fruit=+
"

-- file opened in w+ --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in w+b --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in w+t --
int(18)
int(18)
bool(false)
string(%d) "+=water===fruit=+
"

-- file opened in x+ --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in x+b --
int(18)
int(18)
bool(false)
string(18) "+=water===fruit=+
"

-- file opened in x+t --
int(18)
int(18)
bool(false)
string(%d) "+=water===fruit=+
"

-- file opened in r+ --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in r+b --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in r+t --
int(20)
int(20)
bool(false)
string(%d) "+-water--fruit-air+
"

-- file opened in a+ --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in a+b --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in a+t --
int(20)
int(20)
bool(false)
string(%d) "+-water--fruit-air+
"

-- file opened in w+ --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in w+b --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in w+t --
int(20)
int(20)
bool(false)
string(%d) "+-water--fruit-air+
"

-- file opened in x+ --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in x+b --
int(20)
int(20)
bool(false)
string(20) "+-water--fruit-air+
"

-- file opened in x+t --
int(20)
int(20)
bool(false)
string(%d) "+-water--fruit-air+
"

-- file opened in r+ --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in r+b --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in r+t --
int(24)
int(24)
bool(false)
string(%d) "+-water---fruit---air-+
"

-- file opened in a+ --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in a+b --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in a+t --
int(24)
int(24)
bool(false)
string(%d) "+-water---fruit---air-+
"

-- file opened in w+ --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in w+b --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in w+t --
int(24)
int(24)
bool(false)
string(%d) "+-water---fruit---air-+
"

-- file opened in x+ --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in x+b --
int(24)
int(24)
bool(false)
string(24) "+-water---fruit---air-+
"

-- file opened in x+t --
int(24)
int(24)
bool(false)
string(%d) "+-water---fruit---air-+
"

-- file opened in r+ --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in r+b --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in r+t --
int(26)
int(26)
bool(false)
string(%d) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in a+ --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in a+b --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in a+t --
int(26)
int(26)
bool(false)
string(%d) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in w+ --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in w+b --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in w+t --
int(26)
int(26)
bool(false)
string(%d) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in x+ --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in x+b --
int(26)
int(26)
bool(false)
string(26) "+&""""&:&"&:,:":&,&:,,,,+
"

-- file opened in x+t --
int(26)
int(26)
bool(false)
string(%d) "+&""""&:&"&:,:":&,&:,,,,+
"
Done
