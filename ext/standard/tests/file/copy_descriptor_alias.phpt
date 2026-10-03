--TEST--
copy() does not truncate its source when opening it creates the destination
--SKIPIF--
<?php
if (!is_dir('/proc/self/fd') || !file_exists('/dev/null')) {
    die('skip requires /proc/self/fd and /dev/null');
}
?>
--FILE--
<?php
$source = __DIR__ . '/copy_descriptor_alias.tmp';
file_put_contents($source, 'preserved');
fclose(STDIN);

echo "descriptor alias: ";
var_dump(copy($source, '/proc/self/fd/0'));
echo "source: ";
var_dump(file_get_contents($source));
echo "device: ";
var_dump(copy($source, '/dev/null'));

unlink($source);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/copy_descriptor_alias.tmp');
?>
--EXPECT--
descriptor alias: bool(false)
source: string(9) "preserved"
device: bool(true)
