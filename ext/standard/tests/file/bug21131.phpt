--TEST--
Bug #21131: fopen($filename, "a+") has broken position
--FILE--
<?php # vim600:syn=php:
$filename = tempnam("/tmp", "phpt");

$fp = fopen($filename, "w") or die("can't open $filename for append");
fwrite($fp, "foobar");
fclose($fp);

$fp = fopen($filename, "a+");
rewind($fp);
fpassthru($fp);
fclose($fp);
unlink($filename);
?>
--EXPECT--
foobar
