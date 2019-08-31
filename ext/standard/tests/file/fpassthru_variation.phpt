--TEST--
Test fpassthru() function: Variations
--FILE--
<?php
/*
Prototype: int fpassthru ( resource $handle );
Description: Reads to EOF on the given file pointer from the current position
  and writes the results to the output buffer.
*/

echo "*** Testing fpassthru() function with files ***\n\n";

echo "--- Testing with different offsets ---\n";

$file_name = __DIR__."/passthru_variation.tmp";
$file_write = fopen($file_name, "w");
fwrite($file_write, "1234567890abcdefghijklmnopqrstuvwxyz");
fclose($file_write);

$file_read = fopen($file_name, "r");

$offset_arr = array(
  /* Positive offsets */
  0,
  1,
  5,
  10,
  20,
  30,
  35,
  36,
  70,
  /* Negative offsets, the file pointer should be at the end of file
  to get data */
  -1,
  -5,
  -10,
  -20,
  -35,
  -36,
  -70
);

for( $i=0; $i<count($offset_arr); $i++ ) {
  echo "-- Iteration $i --\n";
  if( $offset_arr[$i] >= 0 ) {
    fseek($file_read, $offset_arr[$i], SEEK_SET);
    var_dump(fpassthru($file_read) );
    rewind( $file_read );
  }else
    {
      fseek($file_read, $offset_arr[$i], SEEK_END);
      var_dump( fpassthru($file_read) );
      rewind( $file_read );
    }
}

fclose($file_read);  // closing the handle

echo "\n--- Testing with binary mode file ---\n";
/* Opening the file in binary read mode */
$file_read = fopen($file_name, "rb");

fseek($file_read, 12, SEEK_SET);
var_dump(fpassthru($file_read) );
rewind( $file_read );
fclose($file_read);

unlink($file_name);

echo "\n*** Done ***\n";

?>
--EXPECT--
*** Testing fpassthru() function with files ***

--- Testing with different offsets ---
-- Iteration 0 --
1234567890abcdefghijklmnopqrstuvwxyzint(36)
-- Iteration 1 --
234567890abcdefghijklmnopqrstuvwxyzint(35)
-- Iteration 2 --
67890abcdefghijklmnopqrstuvwxyzint(31)
-- Iteration 3 --
abcdefghijklmnopqrstuvwxyzint(26)
-- Iteration 4 --
klmnopqrstuvwxyzint(16)
-- Iteration 5 --
uvwxyzint(6)
-- Iteration 6 --
zint(1)
-- Iteration 7 --
int(0)
-- Iteration 8 --
int(0)
-- Iteration 9 --
zint(1)
-- Iteration 10 --
vwxyzint(5)
-- Iteration 11 --
qrstuvwxyzint(10)
-- Iteration 12 --
ghijklmnopqrstuvwxyzint(20)
-- Iteration 13 --
234567890abcdefghijklmnopqrstuvwxyzint(35)
-- Iteration 14 --
1234567890abcdefghijklmnopqrstuvwxyzint(36)
-- Iteration 15 --
1234567890abcdefghijklmnopqrstuvwxyzint(36)

--- Testing with binary mode file ---
cdefghijklmnopqrstuvwxyzint(24)

*** Done ***
