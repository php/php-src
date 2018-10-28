--TEST--
Test file_get_contents() function : variation - various absolute and relative paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only run on Windows");
?>
--FILE--
<?php
/* Prototype  : string file_get_contents(string filename [, bool use_include_path [, resource context [, long offset [, long maxlen]]]])
 * Description: Read the entire file into a string
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing file_get_contents() : variation ***\n";
$mainDir = "fileGetContentsVar7.dir";
$subDir = "fileGetContentsVar7Sub";
$absMainDir = dirname(__FILE__)."\\".$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir."\\".$subDir;
mkdir($absSubDir);

$old_dir_path = getcwd();
chdir(dirname(__FILE__));
$unixifiedDir = '/'.substr(str_replace('\\','/',$absSubDir),3);

$allDirs = array(
  // absolute paths
  "$absSubDir\\",
  "$absSubDir\\..\\".$subDir,
  "$absSubDir\\\\..\\.\\".$subDir,
  "$absSubDir\\..\\..\\".$mainDir."\\.\\".$subDir,
  "$absSubDir\\..\\\\\\".$subDir."\\\\..\\\\..\\".$subDir,
  "$absSubDir\\BADDIR",

  // relative paths
  $mainDir."\\".$subDir,
  $mainDir."\\\\".$subDir,
   $mainDir."\\\\\\".$subDir,
  ".\\".$mainDir."\\..\\".$mainDir."\\".$subDir,
  "BADDIR",

  // unixifed path
  $unixifiedDir,
);

$filename = 'FileGetContentsVar7.tmp';
$absFile = $absSubDir.'/'.$filename;
$h = fopen($absFile,"w");
fwrite($h, "contents read");
fclose($h);

for($i = 0; $i<count($allDirs); $i++) {
  $j = $i+1;
  $dir = $allDirs[$i];
  echo "\n-- Iteration $j --\n";
  var_dump(file_get_contents($dir."\\".$filename));
}

unlink($absFile);
chdir($old_dir_path);
rmdir($absSubDir);
rmdir($absMainDir);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing file_get_contents() : variation ***

-- Iteration 1 --
string(%d) "contents read"

-- Iteration 2 --
string(%d) "contents read"

-- Iteration 3 --
string(%d) "contents read"

-- Iteration 4 --
string(%d) "contents read"

-- Iteration 5 --

Warning: file_get_contents(%sfileGetContentsVar7.dir\fileGetContentsVar7Sub\..\\\fileGetContentsVar7Sub\\..\\..\fileGetContentsVar7Sub\FileGetContentsVar7.tmp): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Iteration 6 --

Warning: file_get_contents(%sfileGetContentsVar7.dir\fileGetContentsVar7Sub\BADDIR\FileGetContentsVar7.tmp): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Iteration 7 --
string(%d) "contents read"

-- Iteration 8 --
string(%d) "contents read"

-- Iteration 9 --
string(%d) "contents read"

-- Iteration 10 --
string(%d) "contents read"

-- Iteration 11 --

Warning: file_get_contents(BADDIR\FileGetContentsVar7.tmp): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Iteration 12 --
string(%d) "contents read"

*** Done ***
