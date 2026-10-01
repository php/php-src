--TEST--
Phar: only .phar extensions in file names trigger automatic archive detection
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$base = __DIR__ . '/' . basename(__FILE__, '.php');
$targets = [
    $base . '.phar.png',
    $base . '.pharabcd/archive.html',
    $base . '.pharma.report.txt',
    $base . '.pharma.phar.html',
];

if (!is_dir(dirname($targets[1]))) {
    mkdir(dirname($targets[1]));
}

foreach ($targets as $i => $target) {
    $source = $base . ".source-$i.phar.zip";
    $constant = "PHAR_STUB_EXECUTED_$i";

    $phar = new Phar($source);
    $phar->addFromString('payload', 'payload');
    $phar->setStub("<?php define('$constant', true); __HALT_COMPILER();");
    $phar->compressFiles(Phar::GZ);
    unset($phar);

    rename($source, $target);

    ob_start();
    include $target;
    ob_end_clean();

    var_dump(defined($constant));
}
?>
--CLEAN--
<?php
$base = __DIR__ . '/' . basename(__FILE__, '.clean.php');
@unlink($base . '.phar.png');
@unlink($base . '.pharabcd/archive.html');
@unlink($base . '.pharma.report.txt');
@unlink($base . '.pharma.phar.html');
for ($i = 0; $i < 4; $i++) {
    @unlink($base . ".source-$i.phar.zip");
}
@rmdir($base . '.pharabcd');
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
