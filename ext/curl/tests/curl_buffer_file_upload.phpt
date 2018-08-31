--TEST--
CURL buffer file uploading
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

function testcurl($ch, $postname, $buffer, $mime = '')
{
	if(!empty($mime)) {
		$file = new CURLBufferFile($postname, $buffer, $mime);
	} else {
		$file = new CURLBufferFile($postname, $buffer);
	}
	curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $file));
	var_dump(curl_exec($ch));
}

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php?test=bufferfile");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$buffer = "test\0test";
var_dump(md5($buffer));
testcurl($ch, 'foo.txt', $buffer);
testcurl($ch, 'foo.txt', $buffer, 'text/plain');

$file = new CurlBufferFile('foo.txt', "test");
$file->setMimeType('text/plain');
var_dump($file->getMimeType());
var_dump($file->getPostFilename());
var_dump($file->getBuffer());
curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $file));
var_dump(curl_exec($ch));

curl_close($ch);
?>
--EXPECTF--
string(%d) "62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|application/octet-stream|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "text/plain"
string(%d) "foo.txt"
string(%d) "test"
string(%d) "foo.txt|text/plain|098f6bcd4621d373cade4e832627b4f6"