--TEST--
Test readfile() function : variation - various invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip run only on Windows");
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
echo "*** Testing readfile() : variation ***\n";

/* An array of files */
$names_arr = array(
  /* Invalid args */
  "-1" => -1,
  "TRUE" => TRUE,
  "FALSE" => FALSE,
  "\"\"" => "",
  "\" \"" => " ",
  "\\0" => "\0",
  "array()" => array(),

  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir" => "/no/such/file/dir",
  "php/php"=> "php/php"

);

foreach($names_arr as $key => $value) {
    echo "\n-- Filename: $key --\n";
    try {
        readfile($value);
    } catch (\TypeError|\ValueError $e) {
        echo get_class($e) . ': ' . $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
*** Testing readfile() : variation ***

-- Filename: -1 --

Warning: readfile(-1): Failed to open stream: No such file or directory in %s on line %d

-- Filename: TRUE --

Warning: readfile(1): Failed to open stream: No such file or directory in %s on line %d

-- Filename: FALSE --
ValueError: Path cannot be empty

-- Filename: "" --
ValueError: Path cannot be empty

-- Filename: " " --

Warning: readfile( ): Failed to open stream: Permission denied in %s on line %d

-- Filename: \0 --
ValueError: readfile(): Argument #1 ($filename) must not contain any null bytes

-- Filename: array() --
TypeError: readfile(): Argument #1 ($filename) must be of type string, array given

-- Filename: /no/such/file/dir --

Warning: readfile(/no/such/file/dir): Failed to open stream: No such file or directory in %s on line %d

-- Filename: php/php --

Warning: readfile(php/php): Failed to open stream: No such file or directory in %s on line %d
