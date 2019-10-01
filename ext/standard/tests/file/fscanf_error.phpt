--TEST--
Test fscanf() function: error conditions
--FILE--
<?php
/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

echo "*** Testing fscanf() for error conditions ***\n";
$file_path = __DIR__;

$filename = "$file_path/fscanf_error.tmp";
$file_handle = fopen($filename, 'w');
if ($file_handle == false)
  exit("Error:failed to open file $filename");
fwrite($file_handle, "hello world");
fclose($file_handle);

// invalid file handle
try {
    var_dump( fscanf($file_handle, "%s") );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// number of formats in format strings not matching the no of variables
$file_handle = fopen($filename, 'r');
if ($file_handle == false)
  exit("Error:failed to open file $filename");
var_dump( fscanf($file_handle, "%d%s%f", $int_var, $string_var) );
fclose($file_handle);

// different invalid format strings
$invalid_formats = array( $undefined_var,
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
$file_path = __DIR__;
$filename = "$file_path/fscanf_error.tmp";
unlink($filename);
?>
--EXPECTF--
*** Testing fscanf() for error conditions ***
fscanf(): supplied resource is not a valid File-Handle resource

Warning: fscanf(): Different numbers of variable names and field specifiers in %s on line %d
int(-1)

Warning: Undefined variable: undefined_var in %s on line %d
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
