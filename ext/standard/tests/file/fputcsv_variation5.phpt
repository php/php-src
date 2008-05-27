--TEST--
Test fputcsv() : usage variations - with default arguments value
--FILE--
<?php
/* 
 Prototype: array fputcsv ( resource $handle , array $fields [, string $delimiter [, string $enclosure]]] );
 Description: Format line as CSV and write to the file pointer 
*/

/* Testing fputcsv() to write to a file when default arguments values are considered */

echo "*** Testing fputcsv() : with default arguments value ***\n";

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
$filename = "$file_path/fputcsv_variation5.tmp";

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
    var_dump( fputcsv($file_handle, $csv_field) );
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
*** Testing fputcsv() : with default arguments value ***

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
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in r+b --
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in r+t --
int(20)
int(20)
bool(false)
string(%d) """"water"",""fruit"
"

-- file opened in a+ --
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in a+b --
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in a+t --
int(20)
int(20)
bool(false)
string(%d) """"water"",""fruit"
"

-- file opened in w+ --
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in w+b --
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in w+t --
int(20)
int(20)
bool(false)
string(%d) """"water"",""fruit"
"

-- file opened in x+ --
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in x+b --
int(20)
int(20)
bool(false)
string(20) """"water"",""fruit"
"

-- file opened in x+t --
int(20)
int(20)
bool(false)
string(%d) """"water"",""fruit"
"

-- file opened in r+ --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in r+b --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in r+t --
int(22)
int(22)
bool(false)
string(%d) """"water"",""fruit"""
"

-- file opened in a+ --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in a+b --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in a+t --
int(22)
int(22)
bool(false)
string(%d) """"water"",""fruit"""
"

-- file opened in w+ --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in w+b --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in w+t --
int(22)
int(22)
bool(false)
string(%d) """"water"",""fruit"""
"

-- file opened in x+ --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in x+b --
int(22)
int(22)
bool(false)
string(22) """"water"",""fruit"""
"

-- file opened in x+t --
int(22)
int(22)
bool(false)
string(%d) """"water"",""fruit"""
"

-- file opened in r+ --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in r+b --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in r+t --
int(18)
int(18)
bool(false)
string(%d) ""^water^ ^fruit^"
"

-- file opened in a+ --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in a+b --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in a+t --
int(18)
int(18)
bool(false)
string(%d) ""^water^ ^fruit^"
"

-- file opened in w+ --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in w+b --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in w+t --
int(18)
int(18)
bool(false)
string(%d) ""^water^ ^fruit^"
"

-- file opened in x+ --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in x+b --
int(18)
int(18)
bool(false)
string(18) ""^water^ ^fruit^"
"

-- file opened in x+t --
int(18)
int(18)
bool(false)
string(%d) ""^water^ ^fruit^"
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
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in r+b --
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in r+t --
int(32)
int(32)
bool(false)
string(%d) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in a+ --
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in a+b --
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in a+t --
int(32)
int(32)
bool(false)
string(%d) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in w+ --
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in w+b --
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in w+t --
int(32)
int(32)
bool(false)
string(%d) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in x+ --
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in x+b --
int(32)
int(32)
bool(false)
string(32) ""&""""""""&:&""&:,:"":&,&:,,,,"
"

-- file opened in x+t --
int(32)
int(32)
bool(false)
string(%d) ""&""""""""&:&""&:,:"":&,&:,,,,"
"
Done
