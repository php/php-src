--TEST--
Bug #77432 (Segmentation fault on including phar file)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php

$filename = __DIR__ . '/bug77432.phar';

$phar = new Phar($filename);
$phar->startBuffering();
$phar->addFromString('test.txt', 'text');
$phar->setStub('<?php echo "hello world\n"; __HALT_COMPILER(); ?>');
$phar->stopBuffering();
unset($phar);

echo "--- Include 1 ---\n";
include("phar://" . $filename);
echo "--- Include 2 ---\n";
// Note: will warn because the halting offset is redefined, but won't display the name because "zend_mangle_property_name" starts the name with \0
// However, this is just the easiest way to reproduce it, so go with this test.
include("phar://" . $filename);
echo "--- After unlink ---\n";
unlink($filename);
// This will just fail, as it should, but it is here to test the reopen error-handling path.
include("phar://" . $filename);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/bug77432.phar');
?>
--EXPECTF--
--- Include 1 ---
hello world
--- Include 2 ---
%A
hello world
--- After unlink ---

Warning: include(%sbug77432.phar): Failed to open stream: phar error: could not reopen phar "%sbug77432.phar" in %s on line %d

Warning: include(): Failed opening '%sbug77432.phar' for inclusion (include_path=%s) in %s on line %d
