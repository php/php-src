--TEST--
Phar::buildFromDirectory() with non-matching regex
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

mkdir(__DIR__.'/testdir6', 0777);
foreach(range(1, 4) as $i) {
    file_put_contents(__DIR__."/testdir6/file$i.txt", "some content for file $i");
}

try {
    $phar = new Phar(__DIR__ . '/buildfromdirectory6.phar');
    var_dump($phar->buildFromDirectory(__DIR__ . '/testdir6', '/\.php$/'));
} catch (Exception $e) {
    var_dump(get_class($e));
    echo $e->getMessage() . "\n";
}

var_dump(file_exists(__DIR__ . '/buildfromdirectory6.phar'));

?>
--CLEAN--
<?php
foreach(range(1, 4) as $i) {
    unlink(__DIR__ . "/testdir6/file$i.txt");
}
rmdir(__DIR__ . '/testdir6');
?>
--EXPECT--
array(0) {
}
bool(false)
