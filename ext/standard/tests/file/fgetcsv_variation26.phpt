--TEST--
Test fgetcsv() : usage variations - reading files opened in write only mode (Bug #42036)
--FILE--
<?php
/*
 Prototype: array fgetcsv ( resource $handle [, int $length [, string $delimiter [, string $enclosure]]] );
 Description: Gets line from file pointer and parse for CSV fields
*/

/* Testing fgetcsv() to read from files opened in write only mode */

echo "*** Testing fgetcsv() : reading the files opened in write only mode ***\n";

/* the array is with three elements in it. Each element should be read as
   1st element is delimiter, 2nd element is enclosure
   and 3rd element is csv fields
*/
$csv_lists = array (
  array(',', '"', '"water",fruit'),
  array(',', '"', '"water","fruit"'),
  array(' ', '^', '^water^ ^fruit^'),
  array(':', '&', '&water&:&fruit&'),
  array('=', '=', '=water===fruit='),
  array('-', '-', '-water--fruit-air'),
  array('-', '-', '-water---fruit---air-'),
  array(':', '&', '&""""&:&"&:,:":&,&:,,,,')
);

$filename = __DIR__ . '/fgetcsv_variation26.tmp';
@unlink($filename);

$file_modes = array ("w", "wb", "wt",
                     "a", "ab", "at",
                     "x", "xb", "xt");

$loop_counter = 1;
foreach ($csv_lists as $csv_list) {
  for($mode_counter = 0; $mode_counter < count($file_modes); $mode_counter++) {
    // create the file and add the content with has csv fields
    $file_handle = fopen($filename, $file_modes[$mode_counter] );

    if ( !$file_handle ) {
      echo "Error: failed to create file $filename!\n";
      exit();
    }
    $delimiter = $csv_list[0];
    $enclosure = $csv_list[1];
    $csv_field = $csv_list[2];

    fwrite($file_handle, $csv_field . "\n");
    // write another line of text and a blank line
    // this will be used to test, if the fgetcsv() read more than a line and its
    // working when only a blank line is read
    fwrite($file_handle, "This is line of text without csv fields\n");
    fwrite($file_handle, "\n"); // blank line

    // rewind the file pointer to bof
      rewind($file_handle);

    echo "\n-- Testing fgetcsv() with file opened using $file_modes[$mode_counter] mode --\n";

    // call fgetcsv() to parse csv fields

    // use the right delimiter and enclosure with max length
    var_dump( fgetcsv($file_handle, 1024, $delimiter, $enclosure) );
    // check the file pointer position and if eof
    var_dump( ftell($file_handle) );
    var_dump( feof($file_handle) );

    // close the file
    fclose($file_handle);
    //delete file
    unlink($filename);
  } //end of mode loop
} // end of foreach

echo "Done\n";
?>
--EXPECTF--
*** Testing fgetcsv() : reading the files opened in write only mode ***

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using w mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using wt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using a mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using ab mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using at mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using x mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xb mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)

-- Testing fgetcsv() with file opened using xt mode --

Notice: fgetcsv(): read of 8192 bytes failed with errno=9 Bad file descriptor in %s on line %d
bool(false)
int(0)
bool(false)
Done
