--TEST--
Test rename() function : variation - various invalid paths
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
/* An array of files */
$names_arr = array(
  /* Invalid args */
  -1, /* -1 is just a valid filename on windows */
  TRUE, /* 1 as well, (string)TRUE > "1" */
  FALSE,
  "", // I think both p8 and php are wrong on the messages here
  //p8 generates different messages to php, php is probably wrong
  //php has either "File Exists" or "Permission Denied".
  " ",

  /* prefix with path separator of a non existing directory*/
  "/no/such/file/dir",
  "php/php"

);

/* disable notice so we don't get the array to string conversion notice for "$name" where $name = array() */
error_reporting(E_ALL ^ E_NOTICE);

echo "*** Testing rename() with obscure files ***\n";
$file_path = __DIR__."/renameVar13";
$aFile = $file_path.'/afile.tmp';

if (!mkdir($file_path)) {
    die("fail to create $file_path tmp dir");
}

for( $i=0; $i < count($names_arr); $i++ ) {
  $name = $names_arr[$i];
  echo "-- $i testing '$name' " . gettype($name) . " --\n";

  touch($aFile);
  var_dump(rename($aFile, $name));
  if (file_exists($name)) {
     @unlink($name);
  }

  if (file_exists($aFile)) {
     @unlink($aFile);
  }
  var_dump(rename($name, $aFile));
  if (file_exists($aFile)) {
     @unlink($aFile);
  }
}

rmdir($file_path);
?>
--EXPECTF--
*** Testing rename() with obscure files ***
-- 0 testing '-1' integer --
bool(true)

Warning: rename(-1,%safile.tmp): The system cannot find the file specified (code: 2) in %srename_variation13-win32.php on line %d
bool(false)
-- 1 testing '1' boolean --
bool(true)

Warning: rename(1,%safile.tmp): The system cannot find the file specified (code: 2) in %srename_variation13-win32.php on line %d
bool(false)
-- 2 testing '' boolean --

Warning: rename(%safile.tmp,): %r(Invalid argument|(The parameter is incorrect|The system cannot find the path specified) \(code: \d+\))%r in %srename_variation13-win32.php on line %d
bool(false)

Warning: rename(,%safile.tmp): %r(Invalid argument|(The parameter is incorrect|The system cannot find the path specified) \(code: \d+\))%r in %srename_variation13-win32.php on line %d
bool(false)
-- 3 testing '' string --

Warning: rename(%safile.tmp,): %r(Invalid argument|(The parameter is incorrect|The system cannot find the path specified) \(code: \d+\))%r in %srename_variation13-win32.php on line %d
bool(false)

Warning: rename(,%safile.tmp): %r(Invalid argument|(The parameter is incorrect|The system cannot find the path specified) \(code: \d+\))%r in %srename_variation13-win32.php on line %d
bool(false)
-- 4 testing ' ' string --

Warning: rename(%s): The filename, directory name, or volume label syntax is incorrect (code: 123) in %srename_variation13-win32.php on line %d
bool(false)

Warning: rename(%s): The filename, directory name, or volume label syntax is incorrect (code: 123) in %srename_variation13-win32.php on line %d
bool(false)
-- 5 testing '/no/such/file/dir' string --

Warning: rename(%safile.tmp,/no/such/file/dir): The system cannot find the path specified (code: 3) in %srename_variation13-win32.php on line %d
bool(false)

Warning: rename(/no/such/file/dir,%safile.tmp): The system cannot find the path specified (code: 3) in %srename_variation13-win32.php on line %d
bool(false)
-- 6 testing 'php/php' string --

Warning: rename(%safile.tmp,php/php): The system cannot find the path specified (code: 3) in %srename_variation13-win32.php on line %d
bool(false)

Warning: rename(php/php,%safile.tmp): The system cannot find the path specified (code: 3) in %srename_variation13-win32.php on line %d
bool(false)
