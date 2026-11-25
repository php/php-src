--TEST--
spl_autoload() frees the static variables of the autoloaded file
--INI--
report_memleaks=1
--FILE--
<?php
$dir = __DIR__ . '/spl_autoload_static_variable_leak_dir';
mkdir($dir);
set_include_path($dir);
file_put_contents($dir . '/stat1.php', '<?php static $top = [1, 2, 3]; class Stat1 {}');

spl_autoload('Stat1');
var_dump(class_exists('Stat1', false));
?>
--CLEAN--
<?php
$dir = __DIR__ . '/spl_autoload_static_variable_leak_dir';
@unlink($dir . '/stat1.php');
@rmdir($dir);
?>
--EXPECT--
bool(true)
