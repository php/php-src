--TEST--
GH-13037 (PharData incorrectly extracts zip file)
--EXTENSIONS--
phar
--FILE--
<?php
$phar = new PharData(__DIR__ . '/files/gh13037.zip');
$phar->extractTo(__DIR__ . '/out_gh13037/');
echo file_get_contents(__DIR__ . '/out_gh13037/test');
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/out_gh13037/test');
@rmdir(__DIR__ . '/out_gh13037');
?>
--EXPECT--
hello
