--TEST--
Test rename() function : variation - various relative, absolute paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip..  for Windows');
?>
--FILE--
<?php
/* Prototype  : bool rename(string old_name, string new_name[, resource context])
 * Description: Rename a file
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

/* Creating unique files in various dirs by passing relative paths to $dir arg */

echo "*** Testing rename() with absolute and relative paths ***\n";
$mainDir = "renameVar11";
$subDir = "renameVar11Sub";
$absMainDir = __DIR__."/".$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir."/".$subDir;
mkdir($absSubDir);

$fromFile = "renameMe.tmp";
$toFile = "IwasRenamed.tmp";

$old_dir_path = getcwd();
chdir(__DIR__);

$allDirs = array(
  // absolute paths
  "$absSubDir/",
  "$absSubDir/../".$subDir,
  "$absSubDir//.././".$subDir,
  "$absSubDir/../../".$mainDir."/./".$subDir,
  "$absSubDir/..///".$subDir."//..//../".$subDir,
  "$absSubDir/BADDIR",


  // relative paths
  $mainDir."/".$subDir,
  $mainDir."//".$subDir,
   $mainDir."///".$subDir,
  "./".$mainDir."/../".$mainDir."/".$subDir,
  "BADDIR",
);

for($i = 0; $i<count($allDirs); $i++) {
  $j = $i+1;
  $dir = $allDirs[$i];
  echo "\n-- Iteration $j --\n";
  touch($absSubDir."/".$fromFile);
  $res = rename($dir."/".$fromFile, $dir."/".$toFile);
  var_dump($res);
  if ($res == true) {
     $res = rename($dir."/".$toFile, $dir."/".$fromFile);
     var_dump($res);
  }
  unlink($absSubDir."/".$fromFile);
}

chdir($old_dir_path);
rmdir($absSubDir);
rmdir($absMainDir);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing rename() with absolute and relative paths ***

-- Iteration 1 --
bool(true)
bool(true)

-- Iteration 2 --
bool(true)
bool(true)

-- Iteration 3 --
bool(true)
bool(true)

-- Iteration 4 --
bool(true)
bool(true)

-- Iteration 5 --

Warning: rename(%s/renameVar11/renameVar11Sub/..///renameVar11Sub//..//../renameVar11Sub/renameMe.tmp,%s/renameVar11/renameVar11Sub/..///renameVar11Sub//..//../renameVar11Sub/IwasRenamed.tmp): The system cannot find the path specified. (code: 3) in %s on line %d
bool(false)

-- Iteration 6 --

Warning: rename(%s/renameVar11/renameVar11Sub/BADDIR/renameMe.tmp,%s/renameVar11/renameVar11Sub/BADDIR/IwasRenamed.tmp): The system cannot find the path specified. (code: 3) in %s on line %d
bool(false)

-- Iteration 7 --
bool(true)
bool(true)

-- Iteration 8 --
bool(true)
bool(true)

-- Iteration 9 --
bool(true)
bool(true)

-- Iteration 10 --
bool(true)
bool(true)

-- Iteration 11 --

Warning: rename(BADDIR/renameMe.tmp,BADDIR/IwasRenamed.tmp): The system cannot find the path specified. (code: 3) in %s on line %d
bool(false)

*** Done ***
