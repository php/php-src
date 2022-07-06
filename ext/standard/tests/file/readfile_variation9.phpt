--TEST--
Test readfile() function : variation - variable types of path
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing readfile() : variation ***\n";
$mainDir = "readfileVar8";
$subDir = "readfileVar8Sub";
$absMainDir = __DIR__."/".$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir."/".$subDir;
mkdir($absSubDir);

$theFile = "fileToRead.tmp";
$absFile = $absSubDir.'/'.$theFile;

// create the file
$h = fopen($absFile,"w");
fwrite($h, "The File Contents");
fclose($h);


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
  $ok = readfile($dir.'/'.$theFile);
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

-- Iteration 1 --
The File Contents
-- Iteration 2 --
The File Contents
-- Iteration 3 --
The File Contents
-- Iteration 4 --
The File Contents
-- Iteration 5 --

Warning: readfile(%sreadfileVar8Sub/..///readfileVar8Sub//..//../readfileVar8Sub/fileToRead.tmp): Failed to open stream: No such file or directory in %s on line %d

-- Iteration 6 --

Warning: readfile(%sreadfileVar8Sub/BADDIR/fileToRead.tmp): Failed to open stream: No such file or directory in %s on line %d

-- Iteration 7 --
The File Contents
-- Iteration 8 --
The File Contents
-- Iteration 9 --
The File Contents
-- Iteration 10 --
The File Contents
-- Iteration 11 --

Warning: readfile(BADDIR/fileToRead.tmp): Failed to open stream: No such file or directory in %s on line %d

*** Done ***
