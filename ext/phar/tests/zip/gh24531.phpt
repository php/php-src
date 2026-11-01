--TEST--
GH-24531 (NULL pointer dereference when flushing a zip-based phar whose file was renamed)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$source = __DIR__ . '/gh24531.phar.zip';
$target = __DIR__ . '/gh24531-renamed.phar.zip';

$phar = new Phar($source);
$phar->addFromString('first', 'first');
unset($phar);

rename($source, $target);

try {
    $phar = new Phar($source);
    $phar->addFromString('second', 'second');
} catch (PharException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24531.phar.zip');
@unlink(__DIR__ . '/gh24531-renamed.phar.zip');
?>
--EXPECTF--
PharException: phar zip flush of "%sgh24531.phar.zip" failed: unable to copy contents of file "first" while creating zip-based phar "%sgh24531.phar.zip"
