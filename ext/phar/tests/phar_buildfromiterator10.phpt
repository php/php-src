--TEST--
Phar::buildFromIterator() RegexIterator(RecursiveIteratorIterator), SplFileInfo as current
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
try {
    $input = __DIR__ . '/buildfromiterator10';
    mkdir($input);
    mkdir($input . '/nested');
    file_put_contents($input . '/input_001.phpt', 'first');
    file_put_contents($input . '/nested/input_002.phpt', 'second');
    file_put_contents($input . '/ignored.txt', 'ignored');

    $phar = new Phar(__DIR__ . '/buildfromiterator10.phar');
    $dir = new RecursiveDirectoryIterator($input);
    $iter = new RecursiveIteratorIterator($dir);
    $a = $phar->buildFromIterator(
        new RegexIterator($iter, '/_\d{3}\.phpt$/'),
        $input . DIRECTORY_SEPARATOR,
    );
    asort($a);
    var_dump($a);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromiterator10.phar');
unlink(__DIR__ . '/buildfromiterator10/input_001.phpt');
unlink(__DIR__ . '/buildfromiterator10/nested/input_002.phpt');
unlink(__DIR__ . '/buildfromiterator10/ignored.txt');
rmdir(__DIR__ . '/buildfromiterator10/nested');
rmdir(__DIR__ . '/buildfromiterator10');
__HALT_COMPILER();
?>
--EXPECTF--
array(2) {
  ["input_001.phpt"]=>
  string(%d) "%sbuildfromiterator10%sinput_001.phpt"
  ["nested/input_002.phpt"]=>
  string(%d) "%sbuildfromiterator10%snested%sinput_002.phpt"
}
