--TEST--
Test mkdir() function : variation: various valid and invalid paths 
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : bool mkdir(string pathname [, int mode [, bool recursive [, resource context]]])
 * Description: Create a directory 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing mkdir() : variation ***\n";

$workDir = "mkdirVar5.tmp";
$subDir = "aSubDir";
mkdir($workDir);
$cwd = getcwd();

$dirs = array(
             // relative
             $workDir.'/'.$subDir,
             './'.$workDir.'/'.$subDir,
             $workDir.'/../'.$workDir.'/'.$subDir,
             
             // relative bad path
             $workDir.'/../BADDIR/'.$subDir,
             'BADDIR/'.$subDir,
             
             //absolute
             $cwd.'/'.$workDir.'/'.$subDir,
             $cwd.'/./'.$workDir.'/'.$subDir,
             $cwd.'/'.$workDir.'/../'.$workDir.'/'.$subDir,

             //absolute bad path             
             $cwd.'/BADDIR/'.$subDir,
             
             //trailing separators
             $workDir.'/'.$subDir.'/',
             $cwd.'/'.$workDir.'/'.$subDir.'/',
             
             // multiple separators
             $workDir.'//'.$subDir,
             $cwd.'//'.$workDir.'//'.$subDir,
             
             );
             

foreach($dirs as $dir) {
   echo "-- creating $dir --\n";           
   $res = mkdir($dir);
   if ($res === true) {
      echo "Directory created\n";
      rmdir($dir);
   }
}

rmdir($workDir);

?>
===DONE===
--EXPECTF--
*** Testing mkdir() : variation ***
-- creating mkdirVar5.tmp/aSubDir --
Directory created
-- creating ./mkdirVar5.tmp/aSubDir --
Directory created
-- creating mkdirVar5.tmp/../mkdirVar5.tmp/aSubDir --
Directory created
-- creating mkdirVar5.tmp/../BADDIR/aSubDir --

Warning: mkdir(): No such file or directory in %s on line %d
-- creating BADDIR/aSubDir --

Warning: mkdir(): No such file or directory in %s on line %d
-- creating %s/mkdirVar5.tmp/aSubDir --
Directory created
-- creating %s/./mkdirVar5.tmp/aSubDir --
Directory created
-- creating %s/mkdirVar5.tmp/../mkdirVar5.tmp/aSubDir --
Directory created
-- creating %s/BADDIR/aSubDir --

Warning: mkdir(): No such file or directory in %s on line %d
-- creating mkdirVar5.tmp/aSubDir/ --
Directory created
-- creating %s/mkdirVar5.tmp/aSubDir/ --
Directory created
-- creating mkdirVar5.tmp//aSubDir --
Directory created
-- creating %s//mkdirVar5.tmp//aSubDir --
Directory created
===DONE===
