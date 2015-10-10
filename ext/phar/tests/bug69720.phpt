--TEST--
Phar - bug #69720 - Null pointer dereference in phar_get_fp_offset()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
    // open an existing phar
    $p = new Phar(__DIR__."/bug69720.phar",0);
    // Phar extends SPL's DirectoryIterator class
	echo $p->getMetadata();
    foreach (new RecursiveIteratorIterator($p) as $file) {
        // $file is a PharFileInfo class, and inherits from SplFileInfo
	$temp="";
        $temp= $file->getFileName() . "\n";
        $temp.=file_get_contents($file->getPathName()) . "\n"; // display contents
	var_dump($file->getMetadata());
    }
}
 catch (Exception $e) {
    echo 'Could not open Phar: ', $e;
}
?>
--EXPECTF--

MY_METADATA_NULL

Warning: file_get_contents(phar://%s): failed to open stream: phar error: "test.php" is not a file in phar "%s.phar" in %s.php on line %d
array(1) {
  ["whatever"]=>
  int(123)
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(26) "2000-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
