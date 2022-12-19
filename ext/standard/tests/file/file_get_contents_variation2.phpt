--TEST--
Test file_get_contents() function : variation - include path testing
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing file_get_contents() : variation ***\n";


require_once('fopen_include_path.inc');

// this doesn't create the include dirs in this directory
// we change to this to ensure we are not part of the
// include paths.
$thisTestDir = "FileGetContentsVar2.dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = 'FileGetContentsVar2.tmp';
$scriptLocFile = __DIR__."/".$filename;

$newpath = create_include_path();
set_include_path($newpath);
runtest();
teardown_include_path();
chdir("..");


function runtest() {
   global $scriptLocFile, $filename;
   $h = fopen($scriptLocFile, "w");
   fwrite($h, "File in script location");
   fclose($h);
   $line = file_get_contents($filename, true);
   echo "$line\n";
   unlink($scriptLocFile);
}

?>
--CLEAN--
<?php
$thisTestDir = "FileGetContentsVar2.dir";
// TODO Clean up tmp files?
rmdir($thisTestDir);
?>
--EXPECT--
*** Testing file_get_contents() : variation ***
File in script location
