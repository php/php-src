--TEST--
Bug #21131: fopen($filename, "a+") has broken position
--FILE--
<?php # vim600:syn=php:
$filename = tempnam("/tmp", "phpt");

$fp = fopen($filename, "w") or die("can't open $filename for append");
fwrite($fp, "foobar");
fclose($fp);

$fp = fopen($filename, "a+");
var_dump(ftell($fp));
rewind($fp);
var_dump(ftell($fp));
fpassthru($fp);
fclose($fp);
unlink($filename);
?>
--EXPECT--
int(6)
int(0)
foobar

