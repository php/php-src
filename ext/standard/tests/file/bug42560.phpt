--TEST--
Bug #42560 Empty directory argument to tempnam yields open_basedir problems
--FILE--
<?php
$tmpdir = sys_get_temp_dir();
ini_set('open_basedir', $tmpdir);
$tempnam = tempnam('', 'test');
var_dump($tempnam !== false);
var_dump(file_exists($tempnam));

if (file_exists($tempnam)) {
    unlink($tempnam);
}
?>
--EXPECT--
bool(true)
bool(true)
