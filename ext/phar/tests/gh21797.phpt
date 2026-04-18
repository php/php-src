--TEST--
GH-21797: Phar::webPhar() NULL dereference when SCRIPT_NAME absent from SAPI environment
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
// The NULL dereference in the CGI/FastCGI branch of webPhar() is not
// reachable from CLI SAPI. This test exercises the CLI code path as a
// regression baseline and verifies the function behaves correctly when
// called outside an HTTP context.

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';

$phar = new Phar($fname);
$phar->addFromString('index.php', '<?php echo "ok\n"; ?>');
$phar->setStub('<?php
Phar::webPhar();
__HALT_COMPILER(); ?>');
unset($phar);

// webPhar() with no HTTP context returns silently (no request method set)
include $fname;
echo "no crash\n";
?>
--CLEAN--
<?php @unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECT--
no crash
