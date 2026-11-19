--TEST--
GH-24631: Delayed OOM raised during compilation is reported even when compilation fails
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') die('skip Zend MM required');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$inc = __DIR__ . '/gh24631_inc.php';
// Huge string literal (forces OOM while compiling the file under a tight
// memory_limit) followed by a stray ')' that would otherwise produce a parse
// error on the OOM-truncated token stream.
file_put_contents($inc, "<?php\n\$a = '" . str_repeat('A', 3 << 19) . "';\n)\n");

$out = shell_exec($php . ' -n -d memory_limit=4M ' . escapeshellarg($inc) . ' 2>&1');
@unlink($inc);

// The real error (OOM) must be reported, and the spurious parse error must not.
var_dump(
    str_contains($out, 'Allowed memory size'),
    str_contains($out, 'Parse error')
);
?>
--EXPECT--
bool(true)
bool(false)
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24631_inc.php');
?>
