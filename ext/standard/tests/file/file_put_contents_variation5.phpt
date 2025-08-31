--TEST--
Test file_put_contents() function : variation - include path testing
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php


$thisTestDir = __DIR__ . '/' .basename(__FILE__, ".php") . ".dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = basename(__FILE__, ".php") . ".tmp";
$scriptLocFile = __DIR__."/".$filename;

$newpath = "rubbish";
set_include_path($newpath);
runtest();
$newpath = "";
set_include_path($newpath);
runtest();
set_include_path("");
runtest();
set_include_path(";;  ; ;c:\\rubbish");
runtest();

chdir(__DIR__);


function runtest() {
   global $scriptLocFile, $filename;
   file_put_contents($filename, "File written in working directory", FILE_USE_INCLUDE_PATH);
   if(file_exists($scriptLocFile)) {
      echo "Fail - this is PHP52 behaviour\n";
      unlink($scriptLocFile);
   }else {
      $line = file_get_contents($filename);
      echo "$line\n";
      unlink($filename);
   }
}
?>
--CLEAN--
<?php
$thisTestDir = __DIR__ . '/' . basename(__FILE__, ".clean.php") . ".dir";
$filename = basename(__FILE__, ".clean.php") . ".tmp";
$scriptLocFile = __DIR__."/".$filename;
@unlink($filename);
@unlink($scriptLocFile);
rmdir($thisTestDir);
?>
--EXPECT--
File written in working directory
File written in working directory
File written in working directory
File written in working directory
