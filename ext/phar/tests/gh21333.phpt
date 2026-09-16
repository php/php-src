--TEST--
GH-21333 (UAF when unlinking entries during iteration of a compressed phar)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
--FILE--
<?php
$phar_path = __DIR__ . "/gh21333.phar";
$phar = new Phar($phar_path);
$phar->addFromString("file", "initial_content");
$phar->addEmptyDir("dir");

$phar2 = $phar->compress(Phar::GZ);

$tmp_src = __DIR__ . "/gh21333.tmp";
file_put_contents($tmp_src, str_repeat("A", 100));

foreach ($phar2 as $item) {
    @copy($tmp_src, $item);
    @unlink($item);
}

$garbage = get_defined_vars();

echo "Done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh21333.phar");
@unlink(__DIR__ . "/gh21333.phar.gz");
@unlink(__DIR__ . "/gh21333.tmp");
?>
--EXPECT--
Done
