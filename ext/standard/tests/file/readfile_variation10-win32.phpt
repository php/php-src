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
/* Prototype  : int readfile(string filename [, bool use_include_path[, resource context]])
 * Description: Output a file or a URL
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing readfile() : variation ***\n";

/* An array of files */
$names_arr = array(
  /* Invalid args */
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

foreach($names_arr as $key => $value) {
    echo "\n-- Filename: $key --\n";
    try {
        readfile($value);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
};

?>
--EXPECTF--
*** Testing readfile() : variation ***

-- Filename: -1 --

Warning: readfile(-1): failed to open stream: No such file or directory in %s on line %d

-- Filename: TRUE --

Warning: readfile(1): failed to open stream: No such file or directory in %s on line %d

-- Filename: FALSE --

Warning: readfile(): Filename cannot be empty in %s on line %d

-- Filename: NULL --

Warning: readfile(): Filename cannot be empty in %s on line %d

-- Filename: "" --

Warning: readfile(): Filename cannot be empty in %s on line %d

-- Filename: " " --

Warning: readfile( ): failed to open stream: Permission denied in %s on line %d

-- Filename: \0 --
readfile() expects parameter 1 to be a valid path, string given

-- Filename: array() --
readfile() expects parameter 1 to be a valid path, array given

-- Filename: /no/such/file/dir --

Warning: readfile(/no/such/file/dir): failed to open stream: No such file or directory in %s on line %d

-- Filename: php/php --

Warning: readfile(php/php): failed to open stream: No such file or directory in %s on line %d
