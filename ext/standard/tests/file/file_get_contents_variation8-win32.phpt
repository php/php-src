--TEST--
Test file_get_contents() function : variation - obscure filenames
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
echo "*** Testing file_get_contents() : variation ***\n";

/* An array of filenames */
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

foreach($names_arr as $key =>$value) {
    echo "\n-- Filename: $key --\n";
    try {
        var_dump(file_get_contents($value));
    } catch (\TypeError|\ValueError $e) {
        echo get_class($e) . ': ' . $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
*** Testing file_get_contents() : variation ***

-- Filename: -1 --

Warning: file_get_contents(-1): Failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Filename: TRUE --

Warning: file_get_contents(1): Failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Filename: FALSE --
ValueError: Path cannot be empty

-- Filename: "" --
ValueError: Path cannot be empty

-- Filename: " " --

Warning: file_get_contents( ): Failed to open stream: Permission denied in %s on line %d
bool(false)

-- Filename: \0 --
ValueError: file_get_contents(): Argument #1 ($filename) must not contain any null bytes

-- Filename: array() --
TypeError: file_get_contents(): Argument #1 ($filename) must be of type string, array given

-- Filename: /no/such/file/dir --

Warning: file_get_contents(/no/such/file/dir): Failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Filename: php/php --

Warning: file_get_contents(php/php): Failed to open stream: No such file or directory in %s on line %d
bool(false)
