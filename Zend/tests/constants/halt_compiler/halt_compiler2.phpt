--TEST--
__HALT_COMPILER(); 2 files
--FILE--
<?php
$text = "<?php echo 'test'; var_dump(__COMPILER_HALT_OFFSET__); __HALT_COMPILER(); ?>
hi there";
file_put_contents(__DIR__ . '/test1.php', $text);
$text = "<?php echo 'test2'; var_dump(__COMPILER_HALT_OFFSET__); __HALT_COMPILER(); ?>
hi there 2";
file_put_contents(__DIR__ . '/test2.php', $text);
include __DIR__ . '/test1.php';
include __DIR__ . '/test2.php';
?>
--CLEAN--
<?php
unlink(__DIR__ . '/test1.php');
unlink(__DIR__ . '/test2.php');
?>
--EXPECT--
testint(73)
test2int(74)
