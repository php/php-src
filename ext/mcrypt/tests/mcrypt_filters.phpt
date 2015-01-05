--TEST--
mcrypt filters
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
foreach (stream_get_filters() as $f) {
	if ($f == "mcrypt.*" || $f == "mdecrypt.*") {
	   echo "FOUND\n";
	}
}

$secretfile = 'secert-file.tmp';
$passphrase = 'My secret';

$iv = substr(md5('iv'.$passphrase, true), 0, 8);
$key = substr(md5('pass1'.$passphrase, true) . 
               md5('pass2'.$passphrase, true), 0, 24);
$opts = array('iv'=>$iv, 'key'=>$key);

$fp = fopen($secretfile, 'wb');
stream_filter_append($fp, 'mcrypt.tripledes', STREAM_FILTER_WRITE, $opts);
fwrite($fp, 'Secret secret secret data');
fclose($fp);

echo md5_file($secretfile)."\n";

$fp = fopen($secretfile, 'rb');
stream_filter_append($fp, 'mdecrypt.tripledes', STREAM_FILTER_READ, $opts);
$data = stream_get_contents($fp);
fclose($fp);

echo $data."\n";

@unlink($secretfile);

--EXPECTF--
FOUND
FOUND
32e14bd3c31f2bd666e4290ebdb166a7
Secret secret secret data