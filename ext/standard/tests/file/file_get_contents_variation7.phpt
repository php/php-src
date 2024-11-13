--TEST--
Test file_get_contents() function : variation - various absolute and relative paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing file_get_contents() : variation ***\n";
$mainDir = "fileGetContentsVar7.dir";
$subDir = "fileGetContentsVar7Sub";
$absMainDir = __DIR__."/".$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir."/".$subDir;
mkdir($absSubDir);

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

$filename = 'FileGetContentsVar7.tmp';
$absFile = $absSubDir.'/'.$filename;
$h = fopen($absFile,"w");
fwrite($h, "contents read");
fclose($h);

for($i = 0; $i<count($allDirs); $i++) {
  $j = $i+1;
  $dir = $allDirs[$i];
  echo "\n-- Iteration $j --\n";
  var_dump(file_get_contents($dir."/".$filename));
}

chdir($old_dir_path);

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$mainDir = "fileGetContentsVar7.dir";
$subDir = "fileGetContentsVar7Sub";
$absMainDir = __DIR__."/".$mainDir;
$absSubDir = $absMainDir."/".$subDir;
$filename = 'FileGetContentsVar7.tmp';
$absFile = $absSubDir.'/'.$filename;
unlink($absFile);
rmdir($absSubDir);
rmdir($absMainDir);
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

Warning: file_get_contents(%sfileGetContentsVar7.dir/fileGetContentsVar7Sub/..///fileGetContentsVar7Sub//..//../fileGetContentsVar7Sub/FileGetContentsVar7.tmp): Failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Iteration 6 --

Warning: file_get_contents(%sfileGetContentsVar7.dir/fileGetContentsVar7Sub/BADDIR/FileGetContentsVar7.tmp): Failed to open stream: No such file or directory in %s on line %d
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

Warning: file_get_contents(BADDIR/FileGetContentsVar7.tmp): Failed to open stream: No such file or directory in %s on line %d
bool(false)

*** Done ***
