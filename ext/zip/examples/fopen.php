<?php
if (!extension_loaded('zip')) {
    dl('zip.so');
}


$fp = fopen('zip://' . dirname(__FILE__) . '/test.zip#test', 'r');
if (!$fp) {
	exit("cannot open\n");
}
while (!feof($fp)) {
	$contents .= fread($fp, 2);
	echo "$contents\n";
}

fclose($fp);
echo "done.\n";


$content = '';
$z = new ZipArchive();
$z->open(dirname(__FILE__) . '/test.zip');
$fp = $z->getStream('test');

var_dump($fp);
if(!$fp) exit("\n");
while (!feof($fp)) {
	$contents .= fread($fp, 2);
}

fclose($fp);
file_put_contents('t',$contents);
echo "done.\n";
