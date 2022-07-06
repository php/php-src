--TEST--
Test gzopen() function : basic functionality for writing
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
echo "*** Testing gzopen() : basic functionality ***\n";


// Initialise all required variables
$filename = "gzopen_basic2.txt.gz";
$modes = array('w', 'w+');
$data = "This was the information that was written";

foreach($modes as $mode) {
   echo "testing mode -- $mode --\n";
   $h = gzopen($filename, $mode);
   if ($h !== false) {
      gzwrite($h, $data);
      gzclose($h);
      $h = gzopen($filename, 'r');
      gzpassthru($h);
      gzclose($h);
      echo "\n";
      unlink($filename);
   }
   else {
      var_dump($h);
   }
}

?>
--EXPECTF--
*** Testing gzopen() : basic functionality ***
testing mode -- w --
This was the information that was written
testing mode -- w+ --

Warning: gzopen(): Cannot open a zlib stream for reading and writing at the same time! in %s on line %d
bool(false)
