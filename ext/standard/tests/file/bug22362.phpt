--TEST--
Bug #22362: combinations of fseek() and fwrite() produce unexpected results.
--FILE--
<?php # vim600:syn=php:
$filename = tempnam("/tmp", "phpt");

$fp = fopen($filename, "w+") or die("can't open $filename for append");
fwrite($fp, "quxbar");
fseek($fp, 3, SEEK_SET);
fread($fp, 1);
fseek($fp, 4, SEEK_SET);
fwrite($fp, '!');
fseek($fp, 0, SEEK_SET);
var_dump(fread($fp, 4095));

ftruncate($fp, 0);
rewind($fp);

fwrite($fp, "barfoo");
fseek($fp, 3, SEEK_SET);
fread($fp, 1);
fwrite($fp, '!');
fseek($fp, 0, SEEK_SET);
var_dump(fread($fp, 4095));

fclose($fp);
unlink($filename);
?>
--EXPECT--
string(6) "quxb!r"
string(6) "barf!o"
