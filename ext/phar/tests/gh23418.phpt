--TEST--
GH-23418: Access a subdirectory of a mounted directory with a trailing slash
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$phar = __DIR__ . '/gh23418.phar';
$mount = __DIR__ . '/gh23418';

@mkdir($mount . '/s2', 0777, true);

$p = new Phar($phar);
$p->addFromString('x.txt', 'x');
$p->setStub('<?php __HALT_COMPILER(); ?>');
unset($p);

$p = new Phar($phar);
Phar::mount('phar://' . $phar . '/m', $mount);
$info = $p['m/s2/'];

echo get_class($info), ', isDir=', $info->isDir() ? 'true' : 'false', PHP_EOL;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23418.phar');
@rmdir(__DIR__ . '/gh23418/s2');
@rmdir(__DIR__ . '/gh23418');
?>
--EXPECT--
PharFileInfo, isDir=true
