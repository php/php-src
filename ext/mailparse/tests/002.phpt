--TEST--
Check stream encoding
--SKIPIF--
<?php if (!extension_loaded("mailparse")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
$text = <<<EOD
hello, this is some text=hello.
EOD;
$fp = tmpfile();
fwrite($fp, $text);
rewind($fp);
$dest = tmpfile();
mailparse_stream_encode($fp, $dest, "quoted-printable");
rewind($dest);
$data = fread($dest, 2048);
echo $data;
?>
--EXPECT--
hello, this is some text=3Dhello.
