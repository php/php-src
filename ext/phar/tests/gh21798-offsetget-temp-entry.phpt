--TEST--
GH-21798: Phar::offsetGet() must free is_temp_dir entry before rejecting .phar/* paths
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$phar = new Phar($fname);
$phar->addFromString('index.php', '<?php echo "ok"; ?>');
unset($phar);

$phar = new Phar($fname);
try {
    $phar->offsetGet('.phar/stub.php');
} catch (BadMethodCallException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $phar->offsetGet('.phar/alias.txt');
} catch (BadMethodCallException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $phar->offsetGet('.phar/internal');
} catch (BadMethodCallException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "no crash\n";
?>
--CLEAN--
<?php @unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECT--
BadMethodCallException: Entry .phar/stub.php does not exist
BadMethodCallException: Entry .phar/alias.txt does not exist
BadMethodCallException: Entry .phar/internal does not exist
no crash
