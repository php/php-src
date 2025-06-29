--TEST--
Test readfile() function : variation - various invalid paths
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
echo "*** Testing readfile() : variation ***\n";


/* An array of files */
$names = [
  /* Invalid args */
  -1,
  "",
  " ",
  "\0",
  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir",
  "php/php",
];

foreach ($names as $name) {;
    echo "-- testing '$name' --\n";
    try {
        readfile($name);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
*** Testing readfile() : variation ***
-- testing '-1' --

Warning: readfile(-1): Failed to open stream: %s in %s on line %d
-- testing '' --
ValueError: Path must not be empty
-- testing ' ' --

Warning: readfile( ): Failed to open stream: %s in %s on line %d
-- testing '%0' --
ValueError: readfile(): Argument #1 ($filename) must not contain any null bytes
-- testing '%sdir' --

Warning: readfile(%sdir): Failed to open stream: %s in %s on line %d
-- testing '%sphp' --

Warning: readfile(%sphp): Failed to open stream: %s in %s on line %d
