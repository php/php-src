--TEST--
CURL file uploading from string
--EXTENSIONS--
curl
--FILE--
<?php

function testcurl($ch, $postname, $data, $mime = null)
{
	if (is_null($mime)) {
		// for default mime value
		$file = new CURLStringFile($data, $postname);
	} else {
		$file = new CURLStringFile($data, $postname, $mime);
	}
	curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $file));
	var_dump(curl_exec($ch));
}

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php?test=string_file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$data = "test\0test";
var_dump(md5($data));
testcurl($ch, 'foo.txt', $data);
testcurl($ch, 'foo.txt', $data, 'text/plain');
testcurl($ch, '', $data);
testcurl($ch, 'foo.txt', '');
testcurl($ch, "foo.txt\0broken_string", $data, "text/plain\0broken_string");

// properties
$file = new CURLStringFile($data, 'foo.txt');
$file->mime = 'text/plain';
var_dump($file->mime);
var_dump($file->postname);
var_dump(md5($file->data));
curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $file));
var_dump(curl_exec($ch));

// serialization / deserialization
$old = new CURLStringFile($data, 'foo.txt', 'text/plain');
$serialized = serialize($old);
$new = unserialize($serialized);
curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $new));
var_dump(curl_exec($ch));

// destroy object before send request
$file = new CURLStringFile($data, 'foo.txt', 'text/plain');
curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $file));
unset($file);
var_dump(curl_exec($ch));

// clone curl handler
$file = new CURLStringFile($data, 'foo.txt', 'text/plain');
curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $file));
$ch2 = clone $ch;
var_dump(curl_exec($ch2));

// properties are references

$file = new CURLStringFile($data, 'foo.txt', 'text/plain');
$data =& $file->data;
$postname =& $file->postname;
$mime =& $file->mime;
curl_setopt($ch, CURLOPT_POSTFIELDS, array("file" => $file));
var_dump(curl_exec($ch));

?>
--EXPECTF--
string(%d) "62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|application/octet-stream|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "error:4"
string(%d) "foo.txt|application/octet-stream|d41d8cd98f00b204e9800998ecf8427e"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "text/plain"
string(%d) "foo.txt"
string(%d) "62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
string(%d) "foo.txt|text/plain|62942c05ed0d1b501c4afe6dc1c4db1b"
