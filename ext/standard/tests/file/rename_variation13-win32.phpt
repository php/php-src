--TEST--
Test rename() function : variation - various invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip run only on Windows");
?>
--FILE--
<?php
/* Prototype  : bool rename(string old_name, string new_name[, resource context])
 * Description: Rename a file 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */


echo "*** Testing rename() with obscure files ***\n";
$file_path = dirname(__FILE__)."/renameVar13";
$aFile = $file_path.'/afile.tmp';

mkdir($file_path);

/* An array of files */ 
$names_arr = array(
  /* Invalid args */ 
  -1,
  TRUE,
  FALSE,
  NULL,
  // I think both p8 and php are wrong on the messages here
  "",
  //p8 generates different messages to php, php is probably wrong
  //php has either "File Exists" or "Permission Denied".
  " ",
  "\0",
  
  // as before
  array(),

  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir", 
  "php/php"

);

for( $i=0; $i<count($names_arr); $i++ ) {
  $name = $names_arr[$i];
  echo "-- testing '$name' --\n";  
  touch($aFile);
  var_dump(rename($aFile, $name));
  if (file_exists($name)) {
     unlink($name);
  }
  if (file_exists($aFile)) {
     unlink($aFile);
  }
  var_dump(rename($name, $aFile));
  if (file_exists($aFile)) {
     unlink($aFile);
  }
}

rmdir($file_path);
echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing rename() with obscure files ***
-- testing '-1' --
bool(true)

Warning: rename(-1,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing '1' --
bool(true)

Warning: rename(1,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing '' --

Warning: rename(%s/renameVar13/afile.tmp,): No such file or directory in %s on line %d
bool(false)

Warning: rename(,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing '' --

Warning: rename(%s/renameVar13/afile.tmp,): No such file or directory in %s on line %d
bool(false)

Warning: rename(,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing '' --

Warning: rename(%s/renameVar13/afile.tmp,): No such file or directory in %s on line %d
bool(false)

Warning: rename(,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing ' ' --

Warning: rename(%s/renameVar13/afile.tmp, ): %s in %s on line %d
bool(false)

Warning: rename( ,%s/renameVar13/afile.tmp): %s in %s on line %d
bool(false)
-- testing '%s' --

Warning: rename(%s/renameVar13/afile.tmp,): %s in %s on line %d
bool(false)

Warning: rename(,%s/renameVar13/afile.tmp): %s in %s on line %d
bool(false)
-- testing 'Array' --

Warning: rename() expects parameter 2 to be string, array given in %s on line %d
bool(false)

Notice: Array to string conversion in %s on line %d

Warning: rename() expects parameter 1 to be string, array given in %s on line %d
bool(false)
-- testing '/no/such/file/dir' --

Warning: rename(%s/renameVar13/afile.tmp,/no/such/file/dir): No such file or directory in %s on line %d
bool(false)

Warning: rename(/no/such/file/dir,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)
-- testing 'php/php' --

Warning: rename(%s/renameVar13/afile.tmp,php/php): No such file or directory in %s on line %d
bool(false)

Warning: rename(php/php,%s/renameVar13/afile.tmp): No such file or directory in %s on line %d
bool(false)

*** Done ***