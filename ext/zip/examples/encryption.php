<?php
error_reporting(E_ALL);
if (!extension_loaded('zip')) {
    dl('zip.so');
}

$name = __DIR__ . '/encrypted.zip';
$pass = 'secret';
$file = 'foo.php';

echo "== Create with per file password\n";

$zip  = new ZipArchive;
$zip->open($name, ZIPARCHIVE::CREATE | ZipArchive::OVERWRITE);
$zip->addFile(__FILE__, $file);
$zip->setEncryptionName($file, ZipArchive::EM_AES_256, $pass);
$zip->close();

echo "== Create with global password\n";

$zip  = new ZipArchive;
$zip->open($name, ZIPARCHIVE::CREATE | ZipArchive::OVERWRITE);
$zip->setPassword($pass);
$zip->addFile(__FILE__, $file);
$zip->setEncryptionName($file, ZipArchive::EM_AES_256);
$zip->close();

echo "== Stat\n";

$zip->open($name);
print_r($zip->statName($file));

echo "== Read\n";

$zip->setPassword($pass);
$text = $zip->getFromName($file);
printf("Size = %d\n", strlen($text));
$zip->close();

echo "== Stream with context\n";

$ctx = stream_context_create(array(
    'zip' => array(
        'password' => $pass
    )
));
$text = file_get_contents("zip://$name#$file", false, $ctx);
printf("Size = %d\n", strlen($text));
