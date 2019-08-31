--TEST--
Test readfile() function : variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only valid for Windows");
?>
--FILE--
<?php
/* Prototype  : int readfile(string filename [, bool use_include_path[, resource context]])
 * Description: Output a file or a URL
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing readfile() : variation ***\n";
$mainDir = "readfileVar8";
$subDir = "readfileVar8Sub";
$absMainDir = __DIR__."\\".$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir."\\".$subDir;
mkdir($absSubDir);

$theFile = "fileToRead.tmp";
$absFile = $absSubDir.'/'.$theFile;

// create the file
$h = fopen($absFile,"w");
fwrite($h, "The File Contents");
fclose($h);


$old_dir_path = getcwd();
chdir(__DIR__);
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

for($i = 0; $i<count($allDirs); $i++) {
  $j = $i+1;
  $dir = $allDirs[$i];
  echo "\n-- $dir --\n";
  $ok = readfile($dir.'\\'.$theFile);
  if ($ok === 1) {
     echo "\n";
  }
}

unlink($absFile);
chdir($old_dir_path);
rmdir($absSubDir);
rmdir($absMainDir);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing readfile() : variation ***

-- %s\readfileVar8\readfileVar8Sub\ --
The File Contents
-- %s\readfileVar8\readfileVar8Sub\..\readfileVar8Sub --
The File Contents
-- %s\readfileVar8\readfileVar8Sub\\..\.\readfileVar8Sub --
The File Contents
-- %s\readfileVar8\readfileVar8Sub\..\..\readfileVar8\.\readfileVar8Sub --
The File Contents
-- %s\readfileVar8\readfileVar8Sub\..\\\readfileVar8Sub\\..\\..\readfileVar8Sub --

Warning: readfile(%s\readfileVar8\readfileVar8Sub\..\\\readfileVar8Sub\\..\\..\readfileVar8Sub\fileToRead.tmp): failed to open stream: No such file or directory in %s on line %d

-- %s\readfileVar8\readfileVar8Sub\BADDIR --

Warning: readfile(%s\readfileVar8\readfileVar8Sub\BADDIR\fileToRead.tmp): failed to open stream: No such file or directory in %s on line %d

-- readfileVar8\readfileVar8Sub --
The File Contents
-- readfileVar8\\readfileVar8Sub --
The File Contents
-- readfileVar8\\\readfileVar8Sub --
The File Contents
-- .\readfileVar8\..\readfileVar8\readfileVar8Sub --
The File Contents
-- BADDIR --

Warning: readfile(BADDIR\fileToRead.tmp): failed to open stream: No such file or directory in %s on line %d

-- /%s/readfileVar8/readfileVar8Sub --
The File Contents
*** Done ***
