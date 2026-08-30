--TEST--
Test fscanf() function: error conditions
--FILE--
<?php
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
    fscanf($file_handle, "%s");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// number of formats in format strings not matching the no of variables
$file_handle = fopen($filename, 'r');
if ($file_handle == false)
  exit("Error:failed to open file $filename");
try {
    fscanf($file_handle, "%d%s%f", $int_var, $string_var);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
fclose($file_handle);

// different invalid format strings
$invalid_formats = array("", "%", "%h", "%.", "%d%m");


// looping to use various invalid formats with fscanf()
foreach($invalid_formats as $format)  {
  $file_handle = fopen($filename, 'r');
  if ($file_handle == false)
    exit("Error:failed to open file $filename");
  try {
    var_dump(fscanf($file_handle, $format));
  } catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
  }
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
--EXPECT--
*** Testing fscanf() for error conditions ***
TypeError: fscanf(): supplied resource is not a valid File-Handle resource
ValueError: Different numbers of variable names and field specifiers
array(0) {
}
ValueError: Bad scan conversion character "
ValueError: Bad scan conversion character "
ValueError: Bad scan conversion character "."
ValueError: Bad scan conversion character "m"

*** Done ***
