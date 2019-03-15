--TEST--
Test fgetcsv() : usage variations - with default enclosure, blank line
--FILE--
<?php
/*
 Prototype: array fgetcsv ( resource $handle [, int $length [, string $delimiter [, string $enclosure]]] );
 Description: Gets line from file pointer and parse for CSV fields
*/

/*
  Testing fgetcsv() to read a file containing blank line when provided with
  default enclosure argument
*/

echo "*** Testing fgetcsv() : with default enclosure, blank line ***\n";

$filename = __DIR__ . '/fgetcsv_variation21.tmp';
@unlink($filename);

$file_modes = array ("r","rb", "rt", "r+", "r+b", "r+t",
                     "a+", "a+b", "a+t",
                     "w+", "w+b", "w+t",
                     "x+", "x+b", "x+t");

$loop_counter = 1;
  for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
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
    // write a blank line
    fwrite($file_handle, "\n"); // blank line

    // close the file if the mode to be used is read mode  and re-open using read mode
    // else rewind the file pointer to beginning of the file
    if ( strstr($file_modes[$mode_counter], "r" ) ) {
      fclose($file_handle);
      $file_handle = fopen($filename, $file_modes[$mode_counter]);
    } else {
      // rewind the file pointer to bof
      rewind($file_handle);
    }

    echo "\n-- Testing fgetcsv() with file opened using $file_modes[$mode_counter] mode --\n";

    // call fgetcsv() to parse csv fields

    // read the line which is a blank line to see the working of fgetcsv
    $fp_pos = ftell($file_handle);
    var_dump( fgetcsv($file_handle, 1024, '+') );
    // check the file pointer position and if eof
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );

    // close the file
    fclose($file_handle);
    //delete file
    unlink($filename);
  } //end of mode loop

echo "Done\n";
?>
--EXPECT--
*** Testing fgetcsv() : with default enclosure, blank line ***

-- Testing fgetcsv() with file opened using r mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using rb mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using rt mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using r+ mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using r+b mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using r+t mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using a+ mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using a+b mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using a+t mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using w+ mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using w+b mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using w+t mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using x+ mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using x+b mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)

-- Testing fgetcsv() with file opened using x+t mode --
array(1) {
  [0]=>
  NULL
}
int(1)
bool(false)
Done
