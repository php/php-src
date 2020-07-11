--TEST--
Test file_put_contents() function : usage variation - various absolute and relative paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing file_put_contents() : usage variation ***\n";

$mainDir = "filePutContentsVar7.dir";
$subDir = "filePutContentsVar7Sub";
$absMainDir = __DIR__."/".$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir."/".$subDir;
mkdir($absSubDir);

$old_dir_path = getcwd();
chdir(__DIR__);


// Note invalid dirs in p8 result in (The system cannot find the path specified.)
// rather than No Such File or Directory in php.net
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
$data = "This was the written data";

for($i = 0; $i<count($allDirs); $i++) {
  $j = $i+1;
  $dir = $allDirs[$i];
  echo "\n-- Iteration $j --\n";
  $res = file_put_contents($dir."/".$filename, ($data . $i));
  if ($res !== false) {
      $in = file_get_contents($absFile);
      if ($in == ($data . $i)) {
         echo "Data written correctly\n";
      }
      else {
         echo "Data not written correctly or to correct place\n";
      }
      unlink($dir."/".$filename);
  }
  else {
     echo "No data written\n";
  }

}

chdir($old_dir_path);
rmdir($absSubDir);
rmdir($absMainDir);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing file_put_contents() : usage variation ***

-- Iteration 1 --
Data written correctly

-- Iteration 2 --
Data written correctly

-- Iteration 3 --
Data written correctly

-- Iteration 4 --
Data written correctly

-- Iteration 5 --

Warning: file_put_contents(%sfilePutContentsVar7.dir/filePutContentsVar7Sub/..///filePutContentsVar7Sub//..//../filePutContentsVar7Sub/FileGetContentsVar7.tmp): Failed to open stream: %s in %s on line %d
No data written

-- Iteration 6 --

Warning: file_put_contents(%sfilePutContentsVar7.dir/filePutContentsVar7Sub/BADDIR/FileGetContentsVar7.tmp): Failed to open stream: %s in %s on line %d
No data written

-- Iteration 7 --
Data written correctly

-- Iteration 8 --
Data written correctly

-- Iteration 9 --
Data written correctly

-- Iteration 10 --
Data written correctly

-- Iteration 11 --

Warning: file_put_contents(BADDIR/FileGetContentsVar7.tmp): Failed to open stream: %s in %s on line %d
No data written

*** Done ***
