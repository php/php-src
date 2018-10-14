--TEST--
Test unlink() function : variation: various valid and invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php
/* Prototype  : bool unlink(string filename[, context context])
 * Description: Delete a file
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing unlink() : variation ***\n";

$workDir = "unlinkVar9.tmp";
$tmpFile = "file.tmp";
chdir(__DIR__);
mkdir($workDir);
$cwd = __DIR__;
$unixifiedFile = '/'.substr(str_replace('\\','/',$cwd).'/'.$workDir.'/'.$tmpFile, 3);

$files = array(
             // relative
             $workDir.'\\'.$tmpFile,
             '.\\'.$workDir.'\\'.$tmpFile,
             $workDir.'\\..\\'.$workDir.'\\'.$tmpFile,

             // relative bad path
             $workDir.'\\..\\BADDIR\\'.$tmpFile,
             'BADDIR\\'.$tmpFile,

             //absolute
             $cwd.'\\'.$workDir.'\\'.$tmpFile,
             $cwd.'\\.\\'.$workDir.'\\'.$tmpFile,
             $cwd.'\\'.$workDir.'\\..\\'.$workDir.'\\'.$tmpFile,

             //absolute bad path
             $cwd.'\\BADDIR\\'.$tmpFile,

             //trailing separators
             $workDir.'\\'.$tmpFile.'\\',
             $cwd.'\\'.$workDir.'\\'.$tmpFile.'\\',

             // multiple separators
             $workDir.'\\\\'.$tmpFile,
             $cwd.'\\\\'.$workDir.'\\\\'.$tmpFile,

             // Unixified File
             $unixifiedFile,

             );


foreach($files as $fileToUnlink) {
	$file = $workDir.'/'.$tmpFile;
	$tounlink = $fileToUnlink;
   touch($file);
   echo "-- removing $tounlink --\n";
   $res = unlink($tounlink);
   if ($res === true) {
      if (file_exists($tounlink) === false) {
      	echo "file removed\n";
      }
      else {
        echo "FAILED: file not removed\n";
      }
   }
   else {
      unlink($file);
   }
}

rmdir($workDir);
?>
===DONE===
--EXPECTF--
*** Testing unlink() : variation ***
-- removing unlinkVar9.tmp\file.tmp --
file removed
-- removing .\unlinkVar9.tmp\file.tmp --
file removed
-- removing unlinkVar9.tmp\..\unlinkVar9.tmp\file.tmp --
file removed
-- removing unlinkVar9.tmp\..\BADDIR\file.tmp --

Warning: unlink(unlinkVar9.tmp\..\BADDIR\file.tmp): No such file or directory in %s on line %d
-- removing BADDIR\file.tmp --

Warning: unlink(BADDIR\file.tmp): No such file or directory in %s on line %d
-- removing %s\unlinkVar9.tmp\file.tmp --
file removed
-- removing %s\.\unlinkVar9.tmp\file.tmp --
file removed
-- removing %s\unlinkVar9.tmp\..\unlinkVar9.tmp\file.tmp --
file removed
-- removing %s\BADDIR\file.tmp --

Warning: unlink(%s\BADDIR\file.tmp): No such file or directory in %s on line %d
-- removing unlinkVar9.tmp\file.tmp\ --

Warning: unlink(unlinkVar9.tmp\file.tmp\): No such file or directory in %s on line %d
-- removing %s\unlinkVar9.tmp\file.tmp\ --

Warning: unlink(%s\unlinkVar9.tmp\file.tmp\): No such file or directory in %s on line %d
-- removing unlinkVar9.tmp\\file.tmp --
file removed
-- removing %s\\unlinkVar9.tmp\\file.tmp --
file removed
-- removing /%s/unlinkVar9.tmp/file.tmp --
file removed
===DONE===
