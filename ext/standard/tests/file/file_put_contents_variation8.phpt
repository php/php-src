--TEST--
Test file_put_contents() function : usage variation - obscure filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Not for Windows");
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
echo "*** Testing file_put_contents() : usage variation ***\n";

$dir = __DIR__ . '/file_put_contents_variation8';
mkdir($dir);
chdir($dir);

/* An array of filenames */
$names_arr = array(
  -1,
  TRUE,
  FALSE,
  NULL,
  "",
  " ",
  //this one also generates a java message rather than our own so we don't replicate php message
  "\0",
  array(),

  //the next 2 generate java messages so we don't replicate the php messages
  "/no/such/file/dir",
  "php/php"

);

for( $i=0; $i<count($names_arr); $i++ ) {
    echo "-- Iteration $i --\n";
    try {
        $res = file_put_contents($names_arr[$i], "Some data");
        if ($res !== false && $res != null) {
            echo "$res bytes written to: '$names_arr[$i]'\n";
            unlink($names_arr[$i]);
        } else {
            echo "Failed to write data to: '$names_arr[$i]'\n";
        }
    } catch (\TypeError|\ValueError $e) {
        echo get_class($e) . ': ' . $e->getMessage(), "\n";
    }
}
rmdir($dir);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing file_put_contents() : usage variation ***
-- Iteration 0 --
9 bytes written to: '-1'
-- Iteration 1 --
9 bytes written to: '1'
-- Iteration 2 --
ValueError: Path cannot be empty
-- Iteration 3 --
ValueError: Path cannot be empty
-- Iteration 4 --
ValueError: Path cannot be empty
-- Iteration 5 --
9 bytes written to: ' '
-- Iteration 6 --
ValueError: file_put_contents(): Argument #1 ($filename) must not contain any null bytes
-- Iteration 7 --
TypeError: file_put_contents(): Argument #1 ($filename) must be of type string, array given
-- Iteration 8 --

Warning: file_put_contents(%sdir): Failed to open stream: %s in %s on line %d
Failed to write data to: '%sir'
-- Iteration 9 --

Warning: file_put_contents(%sphp): Failed to open stream: %s in %s on line %d
Failed to write data to: '%sphp'

*** Done ***
