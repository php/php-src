--TEST--
Test rmdir() function : variation: various valid and invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only valid for Windows");
?>
--FILE--
<?php
echo "*** Testing rmdir() : variation ***\n";

$workDir = "rmdirVar3.tmp";
$subDir = "aSubDir";
mkdir($workDir);
$cwd = getcwd();

$unixifiedDir = '/'.substr(str_replace('\\','/',$cwd).'/'.$workDir.'/'.$subDir, 3);

$dirs = array(
             // relative
             $workDir.'\\'.$subDir,
             '.\\'.$workDir.'\\'.$subDir,
             $workDir.'\\..\\'.$workDir.'\\'.$subDir,

             // relative bad path
             $workDir.'\\..\\BADDIR\\'.$subDir,
             'BADDIR\\'.$subDir,

             //absolute
             $cwd.'\\'.$workDir.'\\'.$subDir,
             $cwd.'\\.\\'.$workDir.'\\'.$subDir,
             $cwd.'\\'.$workDir.'\\..\\'.$workDir.'\\'.$subDir,

             //absolute bad path
             $cwd.'\\BADDIR\\'.$subDir,

             //trailing separators
             $workDir.'\\'.$subDir.'\\',
             $cwd.'\\'.$workDir.'\\'.$subDir.'\\',

             // multiple separators
             $workDir.'\\\\'.$subDir,
             $cwd.'\\\\'.$workDir.'\\\\'.$subDir,

             // Unixified Dir
             $unixifiedDir,

             );


foreach($dirs as $dir) {
   mkdir($workDir.'/'.$subDir);
   echo "-- removing $dir --\n";
   $res = rmdir($dir);
   if ($res === true) {
      echo "Directory removed\n";
   }
   else {
      rmdir($workDir.'/'.$subDir);
   }
}

rmdir($workDir);

?>
--EXPECTF--
*** Testing rmdir() : variation ***
-- removing rmdirVar3.tmp\aSubDir --
Directory removed
-- removing .\rmdirVar3.tmp\aSubDir --
Directory removed
-- removing rmdirVar3.tmp\..\rmdirVar3.tmp\aSubDir --
Directory removed
-- removing rmdirVar3.tmp\..\BADDIR\aSubDir --

Warning: rmdir(rmdirVar3.tmp\..\BADDIR\aSubDir): No such file or directory in %s on line %d
-- removing BADDIR\aSubDir --

Warning: rmdir(BADDIR\aSubDir): No such file or directory in %s on line %d
-- removing %s\rmdirVar3.tmp\aSubDir --
Directory removed
-- removing %s\.\rmdirVar3.tmp\aSubDir --
Directory removed
-- removing %s\rmdirVar3.tmp\..\rmdirVar3.tmp\aSubDir --
Directory removed
-- removing %s\BADDIR\aSubDir --

Warning: rmdir(%s\BADDIR\aSubDir): No such file or directory in %s on line %d
-- removing rmdirVar3.tmp\aSubDir\ --
Directory removed
-- removing %s\rmdirVar3.tmp\aSubDir\ --
Directory removed
-- removing rmdirVar3.tmp\\aSubDir --
Directory removed
-- removing %s\\rmdirVar3.tmp\\aSubDir --
Directory removed
-- removing /%s/rmdirVar3.tmp/aSubDir --
Directory removed
