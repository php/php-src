--TEST--
Test unlink() function : variation: unlinking directories
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
echo "*** Testing unlink() : variation ***\n";

$workDir = "unlinkVar10.tmp";
$tmpDir = "subDir.tmp";
$dirToLinkTo = $workDir.'/'."linkme.tmp";

mkdir($workDir);
$cwd = getcwd();
mkdir($dirToLinkTo);

$dirs = array(
             // relative
             $workDir.'/'.$tmpDir,
             './'.$workDir.'/'.$tmpDir,
             $workDir.'/../'.$workDir.'/'.$tmpDir,

             //absolute
             $cwd.'/'.$workDir.'/'.$tmpDir,
             $cwd.'/./'.$workDir.'/'.$tmpDir,
             $cwd.'/'.$workDir.'/../'.$workDir.'/'.$tmpDir,

             // multiple separators
             $workDir.'//'.$tmpDir,
             $cwd.'//'.$workDir.'//'.$tmpDir,

             );


foreach($dirs as $dirToUnlink) {
   test_link($workDir.'/'.$tmpDir, $dirToLinkTo, $dirToUnlink, true);  //soft link
   //cannot test hard links unless you are root.
}

echo "\n--- try to unlink a directory ---\n";
unlink($dirToLinkTo);
rmdir($dirToLinkTo);
rmdir($workDir);

function test_link($linkedDir, $toLinkTo, $tounlink, $softlink) {
   if ($softlink == true) {
      symlink($toLinkTo, $linkedDir);
      $msg = "soft link";
   }
   else {
      link($toLinkTo, $linkedDir);
      $msg = "hard link";
   }
   echo "-- unlinking $msg $tounlink --\n";
   $res = unlink($tounlink);
   if ($res === true) {
      if (is_link($tounlink) === false) {
        echo "directory unlinked\n";
      }
      else {
        echo "FAILED: directory not unlinked\n";
      }
   }
   else {
      unlink($linkedDir);
   }
}


?>
--EXPECTF--
*** Testing unlink() : variation ***
-- unlinking soft link unlinkVar10.tmp/subDir.tmp --
directory unlinked
-- unlinking soft link ./unlinkVar10.tmp/subDir.tmp --
directory unlinked
-- unlinking soft link unlinkVar10.tmp/../unlinkVar10.tmp/subDir.tmp --
directory unlinked
-- unlinking soft link /%s/unlinkVar10.tmp/subDir.tmp --
directory unlinked
-- unlinking soft link /%s/./unlinkVar10.tmp/subDir.tmp --
directory unlinked
-- unlinking soft link /%s/unlinkVar10.tmp/../unlinkVar10.tmp/subDir.tmp --
directory unlinked
-- unlinking soft link unlinkVar10.tmp//subDir.tmp --
directory unlinked
-- unlinking soft link /%s//unlinkVar10.tmp//subDir.tmp --
directory unlinked

--- try to unlink a directory ---

Warning: unlink(unlinkVar10.tmp/linkme.tmp): %s in %s on line %d
