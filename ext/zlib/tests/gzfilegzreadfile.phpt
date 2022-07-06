--TEST--
gzfile(), gzreadfile()
--SKIPIF--
<?php
if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$original = <<<EOD
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah

EOD;

$filename = tempnam(sys_get_temp_dir(), "phpt");

$fp = gzopen($filename, "wb");
gzwrite($fp, $original);
var_dump(strlen($original));
fclose($fp);

readgzfile($filename);

echo "\n";

$lines = gzfile($filename);

unlink($filename);

foreach ($lines as $line) {
    echo $line;
}

?>
--EXPECT--
int(560)
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah

blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
blah blah blah blah blah blah blah
