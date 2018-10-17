--TEST--
Test fscanf() function: error conditions
--FILE--
<?php
/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

echo "*** Testing fscanf() for error conditions ***\n";
$file_path = dirname(__FILE__);

$filename = "$file_path/fscanf_error.tmp";
$file_handle = fopen($filename, 'w');
if ($file_handle == false)
  exit("Error:failed to open file $filename");
fwrite($file_handle, "hello world");
fclose($file_handle);

// zero argument
var_dump( fscanf() );

// single argument
$file_handle = fopen($filename, 'r');
if ($file_handle == false)
  exit("Error:failed to open file $filename");
var_dump( fscanf($file_handle) );
fclose($file_handle);

// invalid file handle
var_dump( fscanf($file_handle, "%s") );

// number of formats in format strings not matching the no of variables
$file_handle = fopen($filename, 'r');
if ($file_handle == false)
  exit("Error:failed to open file $filename");
var_dump( fscanf($file_handle, "%d%s%f", $int_var, $string_var) );
fclose($file_handle);

// different invalid format strings
$invalid_formats = array( $undefined_var, undefined_constant,
                          "%", "%h", "%.", "%d%m"
                   );


// looping to use various invalid formats with fscanf()
foreach($invalid_formats as $format)  {
  $file_handle = fopen($filename, 'r');
  if ($file_handle == false)
    exit("Error:failed to open file $filename");
  var_dump( fscanf($file_handle, $format) );
  fclose($file_handle);
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fscanf_error.tmp";
unlink($filename);
?>
--EXPECTF--
*** Testing fscanf() for error conditions ***

Warning: fscanf() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: fscanf() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fscanf(): supplied resource is not a valid File-Handle resource in %s on line %d
bool(false)

Warning: fscanf(): Different numbers of variable names and field specifiers in %s on line %d
int(-1)

Notice: Undefined variable: undefined_var in %s on line %d

Warning: Use of undefined constant undefined_constant - assumed 'undefined_constant' (this will throw an Error in a future version of PHP) in %s on line %d
array(0) {
}
array(0) {
}

Warning: fscanf(): Bad scan conversion character " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character "." in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character "m" in %s on line %d
NULL

*** Done ***
