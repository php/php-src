--TEST--
tidy_repair_string() / tidy_repair_file() accept null for $encoding, as declared
--EXTENSIONS--
tidy
--FILE--
<?php
declare(strict_types=1);

$file = __DIR__ . '/tidy_repair_encoding_null.html';
file_put_contents($file, '<p>test');

var_dump(strlen(tidy_repair_string('<p>test', null, null)) > 0);
var_dump(strlen(tidy_repair_file($file, null, null)) > 0);
var_dump(strlen(tidy::repairString('<p>test', null, null)) > 0);
var_dump(strlen(tidy::repairFile($file, null, null)) > 0);

var_dump(tidy_repair_string('<p>test', null, null) === tidy_repair_string('<p>test'));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/tidy_repair_encoding_null.html');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
