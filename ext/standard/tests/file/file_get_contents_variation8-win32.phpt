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
--FILE--
<?php
/* Prototype  : string file_get_contents(string filename [, bool use_include_path [, resource context [, long offset [, long maxlen]]]])
 * Description: Read the entire file into a string 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing file_get_contents() : variation ***\n";

/* An array of filenames */ 
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

foreach($names_arr as $key =>$value) {
  echo "\n-- Filename: $key --\n";
  var_dump(file_get_contents($value));
}

?>
===Done===
--EXPECTF--
*** Testing file_get_contents() : variation ***

-- Filename: -1 --

Warning: file_get_contents(-1): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Filename: TRUE --

Warning: file_get_contents(1): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Filename: FALSE --

Warning: file_get_contents(): Filename cannot be empty in %s on line %d
bool(false)

-- Filename: NULL --

Warning: file_get_contents(): Filename cannot be empty in %sfile_get_contents_variation8-win32.php on line %d
bool(false)

-- Filename: "" --

Warning: file_get_contents(): Filename cannot be empty in %s on line %d
bool(false)

-- Filename: " " --

Warning: file_get_contents( ): failed to open stream: Permission denied in %s on line %d
bool(false)

-- Filename: \0 --

Warning: file_get_contents() expects parameter 1 to be a valid path, string given in %s on line %d
NULL

-- Filename: array() --

Warning: file_get_contents() expects parameter 1 to be a valid path, array given in %s on line %d
NULL

-- Filename: /no/such/file/dir --

Warning: file_get_contents(/no/such/file/dir): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Filename: php/php --

Warning: file_get_contents(php/php): failed to open stream: No such file or directory in %s on line %d
bool(false)
===Done===
