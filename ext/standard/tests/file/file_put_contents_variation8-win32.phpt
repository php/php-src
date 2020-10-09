--TEST--
Test file_put_contents() function : usage variation - obscure filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only run on Windows");
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
echo "*** Testing file_put_contents() : usage variation ***\n";

/* An array of filenames */
$names_arr = array(
  "-1" => -1,
  "TRUE" => TRUE,
  "FALSE" => FALSE,
  "NULL" => NULL,
  "\"\"" => "",
  "\" \"" => " ",
  "\\0" => "\0",
  "array()" => array(),

  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir" => "/no/such/file/dir",
  "php/php"=> "php/php"

);

foreach($names_arr as $key =>$value) {
    echo "\n-- Filename: $key --\n";
    try {
        $res = file_put_contents($value, "Some data");
        if ($res !== false && $res != null) {
            echo "$res bytes written to: '$value'\n";
            unlink($value);
        } else {
            echo "Failed to write data to: $key\n";
        }
    } catch (\TypeError|\ValueError $e) {
        echo get_class($e) . ': ' . $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
*** Testing file_put_contents() : usage variation ***

-- Filename: -1 --
9 bytes written to: '-1'

-- Filename: TRUE --
9 bytes written to: '1'

-- Filename: FALSE --
ValueError: Path cannot be empty

-- Filename: NULL --
ValueError: Path cannot be empty

-- Filename: "" --
ValueError: Path cannot be empty

-- Filename: " " --

Warning: file_put_contents( ): Failed to open stream: Permission denied in %s on line %d
Failed to write data to: " "

-- Filename: \0 --
ValueError: file_put_contents(): Argument #1 ($filename) must not contain any null bytes

-- Filename: array() --
TypeError: file_put_contents(): Argument #1 ($filename) must be of type string, array given

-- Filename: /no/such/file/dir --

Warning: file_put_contents(/no/such/file/dir): Failed to open stream: %s in %s on line %d
Failed to write data to: /no/such/file/dir

-- Filename: php/php --

Warning: file_put_contents(php/php): Failed to open stream: %s in %s on line %d
Failed to write data to: php/php
