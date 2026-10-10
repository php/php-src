--TEST--
DOS device prefixed paths resolve junctions and respect open_basedir
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) !== "WIN") {
    die("skip Windows only");
}
?>
--FILE--
<?php

$sep = DIRECTORY_SEPARATOR;
$dir = __DIR__ . $sep . 'dos_device_prefix';
$allowed = $dir . $sep . 'allowed';
$secret = $dir . $sep . 'secret';
$junction = $allowed . $sep . 'junction';

mkdir($allowed, 0777, true);
mkdir($secret);
file_put_contents($allowed . $sep . 'ok.txt', 'ok');
file_put_contents($secret . $sep . 'file.txt', 'secret');
exec(sprintf('mklink /J "%s" "%s"', $junction, $secret), $output, $status);
if ($status !== 0) {
    die("mklink failed: " . implode("\n", $output));
}

$plain = $junction . $sep . 'file.txt';
$spellings = [
    'plain' => $plain,
    'dot' => '\\\\.\\' . $plain,
    'question' => '\\\\?\\' . $plain,
];
$expected = $secret . $sep . 'file.txt';

echo "Junction resolves through all spellings:\n";
foreach ($spellings as $name => $path) {
    echo "$name: ";
    var_dump(realpath($path) === $expected, file_get_contents($path));
}

echo "Prefixed paths without a drive letter are left alone:\n";
$h = fopen('\\\\.\\NUL', 'wb');
var_dump(is_resource($h));
fclose($h);

echo "Reserved names are still rejected:\n";
var_dump(@fopen('\\\\.\\' . $allowed . $sep . 'NUL', 'wb') === false);
var_dump(file_exists('\\\\?\\' . $allowed . $sep . 'NUL'));

echo "open_basedir sees the same path for all spellings:\n";
ini_set('open_basedir', $allowed);
foreach ($spellings as $name => $path) {
    echo "$name: ";
    var_dump(@file_get_contents('\\\\.\\' . $allowed . $sep . 'ok.txt') === 'ok');
    var_dump(@file_get_contents($path) === false);
    var_dump(@file_get_contents(str_replace($junction, $secret, $path)) === false);
}

?>
--CLEAN--
<?php
$dir = __DIR__ . DIRECTORY_SEPARATOR . 'dos_device_prefix';
exec(sprintf('rmdir /S /Q "%s"', $dir));
?>
--EXPECT--
Junction resolves through all spellings:
plain: bool(true)
string(6) "secret"
dot: bool(true)
string(6) "secret"
question: bool(true)
string(6) "secret"
Prefixed paths without a drive letter are left alone:
bool(true)
Reserved names are still rejected:
bool(true)
bool(false)
open_basedir sees the same path for all spellings:
plain: bool(true)
bool(true)
bool(true)
dot: bool(true)
bool(true)
bool(true)
question: bool(true)
bool(true)
bool(true)
