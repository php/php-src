--TEST--
Test readfile() function : variation - various invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip run only on Windows");
?>
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
      readfile($value);
};

?>
===Done===
--EXPECTF--
*** Testing readfile() : variation ***

-- Filename: -1 --

Warning: readfile(-1): failed to open stream: No such file or directory in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: TRUE --

Warning: readfile(1): failed to open stream: No such file or directory in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: FALSE --

Warning: readfile(): Filename cannot be empty in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: NULL --

Warning: readfile(): Filename cannot be empty in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: "" --

Warning: readfile(): Filename cannot be empty in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: " " --

Warning: readfile() expects parameter 1 to be a valid path, string given in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: \0 --

Warning: readfile() expects parameter 1 to be a valid path, string given in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: array() --

Warning: readfile() expects parameter 1 to be a valid path, array given in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: /no/such/file/dir --

Warning: readfile(/no/such/file/dir): failed to open stream: No such file or directory in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30

-- Filename: php/php --

Warning: readfile(php/php): failed to open stream: No such file or directory in G:\php-sdk\php-src\branches\PHP_5_4\ext\standard\tests\file\readfile_variation10-win32.php on line 30
===Done===

