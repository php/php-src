--TEST--
Test file_put_contents() function : usage variation - various absolute and relative paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing file_put_contents() : usage variation ***\n";

$mainDir = "filePutContentsVar7.dir";
$subDir = "filePutContentsVar7Sub";
$absMainDir = __DIR__.DIRECTORY_SEPARATOR.$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir.DIRECTORY_SEPARATOR.$subDir;
mkdir($absSubDir);

$old_dir_path = getcwd();
chdir(__DIR__);


// Note invalid dirs in p8 result in (The system cannot find the path specified.)
// rather than No Such File or Directory in php.net
$allDirs = [
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
];

$allDirs = str_replace('/', DIRECTORY_SEPARATOR, $allDirs);

$unixifiedDir = '/'. substr(str_replace(DIRECTORY_SEPARATOR, '/', $absSubDir), 3);
$allDirs[] = $unixifiedDir;

$filename = 'FileGetContentsVar7.tmp';
$absFile = $absSubDir. DIRECTORY_SEPARATOR .$filename;
$data = "This was the written data";

$i = 0;
foreach ($allDirs as $dir) {
    $res = file_put_contents($dir.DIRECTORY_SEPARATOR.$filename, ($data . $i));
    if ($res !== false) {
        $in = file_get_contents($absFile);
        if ($in == ($data . $i)) {
           echo "Data written correctly\n";
        }
        else {
           echo "Data not written correctly or to correct place\n";
        }
        unlink($dir.DIRECTORY_SEPARATOR.$filename);
    }
    else {
       echo "No data written\n";
    }
    ++$i;
}

chdir($old_dir_path);

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$mainDir = "filePutContentsVar7.dir";
$subDir = "filePutContentsVar7Sub";
$absMainDir = __DIR__.DIRECTORY_SEPARATOR.$mainDir;
$absSubDir = $absMainDir.DIRECTORY_SEPARATOR.$subDir;
$filename = 'FileGetContentsVar7.tmp';
$absFile = $absSubDir. DIRECTORY_SEPARATOR .$filename;
@unlink($absFile);
rmdir($absSubDir);
rmdir($absMainDir);
?>
--EXPECTF--
*** Testing file_put_contents() : usage variation ***
Data written correctly
Data written correctly
Data written correctly
Data written correctly

Warning: file_put_contents(%sfilePutContentsVar7.dir%efilePutContentsVar7Sub%e..%e%e%efilePutContentsVar7Sub%e%e..%e%e..%efilePutContentsVar7Sub%eFileGetContentsVar7.tmp): Failed to open stream: %s in %s on line %d
No data written

Warning: file_put_contents(%sfilePutContentsVar7.dir%efilePutContentsVar7Sub%eBADDIR%eFileGetContentsVar7.tmp): Failed to open stream: %s in %s on line %d
No data written
Data written correctly
Data written correctly
Data written correctly
Data written correctly

Warning: file_put_contents(BADDIR%eFileGetContentsVar7.tmp): Failed to open stream: %s in %s on line %d
No data written

Warning: file_put_contents(%sfilePutContentsVar7.dir%efilePutContentsVar7Sub%eFileGetContentsVar7.tmp): Failed to open stream: No such file or directory in %s on line %d
No data written

*** Done ***
