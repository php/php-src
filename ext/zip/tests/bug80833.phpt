--TEST--
Bug #80833 (ZipArchive::getStream doesn't use setPassword)
--SKIPIF--
<?php
if (!extension_loaded('zip')) die("skip zip extension not available");
if (!method_exists('ZipArchive', 'setEncryptionName')) die('skip encryption not supported');
?>
--FILE--
<?php
$create_zip = new ZipArchive();
$create_zip->open(__DIR__ . "/80833.zip", ZipArchive::CREATE);
$create_zip->setPassword("default_password");
$create_zip->addFromString("test.txt", "This is a test string.");
$create_zip->setEncryptionName("test.txt", ZipArchive::EM_AES_256, "first_password");
$create_zip->addFromString("test2.txt", "This is another test string.");
$create_zip->setEncryptionName("test2.txt", ZipArchive::EM_AES_256, "second_password");
$create_zip->close();

// Stream API
$o = [
	'zip' => [
		'password' => "first_password",
	],
];
$c = stream_context_create($o);
var_dump(file_get_contents("zip://" . __DIR__ . "/80833.zip#test.txt", false, $c));

// getStream method
$extract_zip = new ZipArchive();
$extract_zip->open(__DIR__ . "/80833.zip", ZipArchive::RDONLY);
$extract_zip->setPassword("first_password");
$file_stream = $extract_zip->getStream("test.txt");
var_dump(stream_get_contents($file_stream));
fclose($file_stream);
$extract_zip->setPassword("second_password");
$file_stream = $extract_zip->getStream("test2.txt");
var_dump(stream_get_contents($file_stream));
fclose($file_stream);
$extract_zip->close();
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/80833.zip");
?>
--EXPECT--
string(22) "This is a test string."
string(22) "This is a test string."
string(28) "This is another test string."
