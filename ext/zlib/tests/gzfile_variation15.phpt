--TEST--
Test gzfile() function : variation: use include path (relative directories in path)
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?>
--FILE--
<?php
require_once('reading_include_path.inc');

//define the files to go into these directories, create one in dir2
set_include_path($newIncludePath);   
test_gzfile();
restore_include_path();

// remove the directory structure
chdir($baseDir);
rmdir($workingDir);
foreach($newdirs as $newdir) {
   rmdir($newdir);
}

chdir("..");
rmdir($thisTestDir);

function test_gzfile() {
   global $scriptFile, $secondFile, $firstFile, $filename;
   
   // create a file in the middle directory
   $h = gzopen($secondFile, "w");
   gzwrite($h, "This is a file in dir2");
   gzclose($h);

   // should read dir2 file
   var_dump(gzfile($filename, true));
   echo "\n";

   //create a file in dir1
   $h = gzopen($firstFile, "w");
   gzwrite($h, "This is a file in dir1");
   gzclose($h);
   
   //should now read dir1 file
   var_dump(gzfile($filename, true));
   echo "\n";
   
   // create a file in working directory
   $h = gzopen($filename, "w");
   gzwrite($h, "This is a file in working dir");
   gzclose($h);
   
   //should still read dir1 file
   var_dump(gzfile($filename, true));
   echo "\n";
   
   unlink($firstFile);
   unlink($secondFile);
   
   //should read the file in working directory
   var_dump(gzfile($filename, true));
   echo "\n";
   
   // create a file in the script directory
   $h = gzopen($scriptFile, "w");
   gzwrite($h, "This is a file in script dir");
   gzclose($h);
   
   //should read the file in script dir
   var_dump(gzfile($filename, true));
   echo "\n";
     
   //cleanup
   unlink($filename);
   unlink($scriptFile);

}

?>
===DONE===
--EXPECTF--
array(1) {
  [0]=>
  string(22) "This is a file in dir2"
}

array(1) {
  [0]=>
  string(22) "This is a file in dir1"
}

array(1) {
  [0]=>
  string(22) "This is a file in dir1"
}

array(1) {
  [0]=>
  string(29) "This is a file in working dir"
}

array(1) {
  [0]=>
  string(28) "This is a file in script dir"
}

===DONE===

