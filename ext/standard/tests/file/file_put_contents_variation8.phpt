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
$names = [
  /* Invalid args */
  '-1',
  "",
  " ",
  "\0",
  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir",
  "php/php",
];

foreach ($names as $name) {
    try {
        $res = file_put_contents($name, "Some data");
        if ($res !== false && $res != null) {
            echo "$res bytes written to: '$name'\n";
            unlink($name);
        } else {
            echo "Failed to write data to: '$name'\n";
        }
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/file_put_contents_variation8';
// TODO Cleanup temp files?
rmdir($dir);
?>
--EXPECTF--
*** Testing file_put_contents() : usage variation ***
9 bytes written to: '-1'
ValueError: Path must not be empty
9 bytes written to: ' '
ValueError: file_put_contents(): Argument #1 ($filename) must not contain any null bytes

Warning: file_put_contents(%sdir): Failed to open stream: %s in %s on line %d
Failed to write data to: '%sir'

Warning: file_put_contents(%sphp): Failed to open stream: %s in %s on line %d
Failed to write data to: '%sphp'

*** Done ***
