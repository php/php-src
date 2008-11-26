--TEST--
Test unlink() function : variation: various valid and invalid paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not for Windows');
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

$workDir = "unlinkVar8.tmp";
$tmpFile = "file.tmp";
$fileToLinkTo = $workDir.'/'."linkme.tmp";

mkdir($workDir);
$cwd = getcwd();
touch($fileToLinkTo);

$files = array(
             // relative
             $workDir.'/'.$tmpFile,
             './'.$workDir.'/'.$tmpFile,
             $workDir.'/../'.$workDir.'/'.$tmpFile,
             
             // relative bad path
             $workDir.'/../BADDIR/'.$tmpFile,
             'BADDIR/'.$tmpFile,
             
             //absolute
             $cwd.'/'.$workDir.'/'.$tmpFile,
             $cwd.'/./'.$workDir.'/'.$tmpFile,
             $cwd.'/'.$workDir.'/../'.$workDir.'/'.$tmpFile,

             //absolute bad path             
             $cwd.'/BADDIR/'.$tmpFile,
             
             //trailing separators
             $workDir.'/'.$tmpFile.'/',
             $cwd.'/'.$workDir.'/'.$tmpFile.'/',
             
             // multiple separators
             $workDir.'//'.$tmpFile,
             $cwd.'//'.$workDir.'//'.$tmpFile,
             
             );
             

foreach($files as $fileToUnlink) {
   test_realfile($workDir.'/'.$tmpFile, $fileToUnlink);
   test_link($workDir.'/'.$tmpFile, $fileToLinkTo, $fileToUnlink, true);  //soft link
   test_link($workDir.'/'.$tmpFile, $fileToLinkTo, $fileToUnlink, false); //hard link   
}

unlink($fileToLinkTo);
rmdir($workDir);

function test_realfile($file, $tounlink) {
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

function test_link($linkedfile, $toLinkTo, $tounlink, $softlink) {
   if ($softlink == true) {
   	  symlink($toLinkTo, $linkedfile);
   	  $msg = "soft link";
   }
   else {
   	  link($toLinkTo, $linkedfile);
   	  $msg = "hard link";   	  
   }   
   echo "-- unlinking $msg $tounlink --\n";           
   $res = unlink($tounlink);
   if ($res === true) {
      if (file_exists($tounlink) === false) {
      	echo "file unlinked\n";
      }
      else {
        echo "FAILED: file not unlinked\n";
      }
   }
   else {
      unlink($linkedfile);
   }
}


?>
===DONE===
--EXPECTF--
*** Testing unlink() : variation ***
-- removing unlinkVar8.tmp/file.tmp --
file removed
-- unlinking soft link unlinkVar8.tmp/file.tmp --
file unlinked
-- unlinking hard link unlinkVar8.tmp/file.tmp --
file unlinked
-- removing ./unlinkVar8.tmp/file.tmp --
file removed
-- unlinking soft link ./unlinkVar8.tmp/file.tmp --
file unlinked
-- unlinking hard link ./unlinkVar8.tmp/file.tmp --
file unlinked
-- removing unlinkVar8.tmp/../unlinkVar8.tmp/file.tmp --
file removed
-- unlinking soft link unlinkVar8.tmp/../unlinkVar8.tmp/file.tmp --
file unlinked
-- unlinking hard link unlinkVar8.tmp/../unlinkVar8.tmp/file.tmp --
file unlinked
-- removing unlinkVar8.tmp/../BADDIR/file.tmp --

Warning: unlink(unlinkVar8.tmp/../BADDIR/file.tmp): No such file or directory in %s on line %d
-- unlinking soft link unlinkVar8.tmp/../BADDIR/file.tmp --

Warning: unlink(unlinkVar8.tmp/../BADDIR/file.tmp): No such file or directory in %s on line %d
-- unlinking hard link unlinkVar8.tmp/../BADDIR/file.tmp --

Warning: unlink(unlinkVar8.tmp/../BADDIR/file.tmp): No such file or directory in %s on line %d
-- removing BADDIR/file.tmp --

Warning: unlink(BADDIR/file.tmp): No such file or directory in %s on line %d
-- unlinking soft link BADDIR/file.tmp --

Warning: unlink(BADDIR/file.tmp): No such file or directory in %s on line %d
-- unlinking hard link BADDIR/file.tmp --

Warning: unlink(BADDIR/file.tmp): No such file or directory in %s on line %d
-- removing /%s/unlinkVar8.tmp/file.tmp --
file removed
-- unlinking soft link /%s/unlinkVar8.tmp/file.tmp --
file unlinked
-- unlinking hard link /%s/unlinkVar8.tmp/file.tmp --
file unlinked
-- removing /%s/./unlinkVar8.tmp/file.tmp --
file removed
-- unlinking soft link /%s/./unlinkVar8.tmp/file.tmp --
file unlinked
-- unlinking hard link /%s/./unlinkVar8.tmp/file.tmp --
file unlinked
-- removing /%s/unlinkVar8.tmp/../unlinkVar8.tmp/file.tmp --
file removed
-- unlinking soft link /%s/unlinkVar8.tmp/../unlinkVar8.tmp/file.tmp --
file unlinked
-- unlinking hard link /%s/unlinkVar8.tmp/../unlinkVar8.tmp/file.tmp --
file unlinked
-- removing /%s/BADDIR/file.tmp --

Warning: unlink(/%s/BADDIR/file.tmp): No such file or directory in %s on line %d
-- unlinking soft link /%s/BADDIR/file.tmp --

Warning: unlink(/%s/BADDIR/file.tmp): No such file or directory in %s on line %d
-- unlinking hard link /%s/BADDIR/file.tmp --

Warning: unlink(/%s/BADDIR/file.tmp): No such file or directory in %s on line %d
-- removing unlinkVar8.tmp/file.tmp/ --

Warning: unlink(unlinkVar8.tmp/file.tmp/): Not a directory in %s on line %d
-- unlinking soft link unlinkVar8.tmp/file.tmp/ --

Warning: unlink(unlinkVar8.tmp/file.tmp/): Not a directory in %s on line %d
-- unlinking hard link unlinkVar8.tmp/file.tmp/ --

Warning: unlink(unlinkVar8.tmp/file.tmp/): Not a directory in %s on line %d
-- removing /%s/unlinkVar8.tmp/file.tmp/ --

Warning: unlink(/%s/unlinkVar8.tmp/file.tmp/): Not a directory in %s on line %d
-- unlinking soft link /%s/unlinkVar8.tmp/file.tmp/ --

Warning: unlink(/%s/unlinkVar8.tmp/file.tmp/): Not a directory in %s on line %d
-- unlinking hard link /%s/unlinkVar8.tmp/file.tmp/ --

Warning: unlink(/%s/unlinkVar8.tmp/file.tmp/): Not a directory in %s on line %d
-- removing unlinkVar8.tmp//file.tmp --
file removed
-- unlinking soft link unlinkVar8.tmp//file.tmp --
file unlinked
-- unlinking hard link unlinkVar8.tmp//file.tmp --
file unlinked
-- removing /%s//unlinkVar8.tmp//file.tmp --
file removed
-- unlinking soft link /%s//unlinkVar8.tmp//file.tmp --
file unlinked
-- unlinking hard link /%s//unlinkVar8.tmp//file.tmp --
file unlinked
===DONE===