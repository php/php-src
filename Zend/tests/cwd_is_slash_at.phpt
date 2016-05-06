--TEST--
Test IS_SLASH_AT for multibyte path
--SKIPIF--
<?php 
include "skipif.inc"; 
if (substr(PHP_OS, 0, 3) != 'WIN' || !extension_loaded('mbstring')) {
	die ("skip only for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

if (setlocale(LC_CTYPE, 'Japanese_Japan.932') === false) {
    echo 'Failure: setlocale', PHP_EOL;
}
// Create "表/ン/機能.php"
shell_exec(
    mb_convert_encoding(
        'mkdir "表/ン" & echo ^<?php echo __FILE__, PHP_EOL, dirname(__FILE__);> "表/ン/機能.php"',
        'CP932',
        'UTF-8'
    )
);
$path = mb_convert_encoding('表/ン/機能.php', 'CP932', 'UTF-8');
$contents = "<?php echo __FILE__, PHP_EOL, dirname(__FILE__);\r\n";
if (file_get_contents($path) !== $contents) {
    echo 'Failure: file_get_contents', PHP_EOL;
}
$file = file($path);
if (count($file) !== 1 || $file[0] !== $contents) {
    echo 'Failure: file', PHP_EOL;
}
$fp = fopen($path, 'r');
if ($fp === false) {
    echo 'Failure: fopen', PHP_EOL;
} else {
    fclose($fp);
}
try {
    $splFileObject = new \SplFileObject($path);
    echo mb_convert_encoding($splFileObject->getPathname(), 'UTF-8', 'CP932'), PHP_EOL;
} catch (\RuntimeException $e) {
    echo 'Failure: SplFileObject', PHP_EOL;
}
echo mb_convert_encoding(
    shell_exec(mb_convert_encoding($php.' "表/ン/機能.php"', 'CP932', 'UTF-8')),
    'UTF-8',
    'CP932'
), PHP_EOL;
//shell_exec(mb_convert_encoding('rmdir /S /Q "表"', 'CP932', 'UTF-8'));
?>
--EXPECTF--
表/ン/機能.php
%s表\ン\機能.php
%s表\ン
