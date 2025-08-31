--TEST--
Phar: tar with huge filenames
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.tar';
$fname3 = __DIR__ . '/' . basename(__FILE__, '.php') . '.3.tar';
$fname4 = __DIR__ . '/' . basename(__FILE__, '.php') . '.4.tar';
$pname = 'phar://' . $fname;

$p1 = new PharData($fname);
$p1[str_repeat('a', 100) . '/b'] = 'hi';
$p1[str_repeat('a', 155) . '/' . str_repeat('b', 100)] = 'hi2';
copy($fname, $fname2);
$p2 = new PharData($fname2);
echo $p2[str_repeat('a', 100) . '/b']->getContent() . "\n";
echo $p2[str_repeat('a', 155) . '/' . str_repeat('b', 100)]->getContent() . "\n";

try {
    $p2[str_repeat('a', 400)] = 'yuck';
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    $p2 = new PharData($fname3);
    $p2[str_repeat('a', 101)] = 'yuck';
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    $p2 = new PharData($fname4);
    $p2[str_repeat('b', 160) . '/' . str_repeat('a', 90)] = 'yuck';
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.tar');
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.3.tar');
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.4.tar');
?>
--EXPECTF--
hi
hi2
tar-based phar "%sbignames.2.tar" cannot be created, filename "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" is too long for tar file format
tar-based phar "%sbignames.3.tar" cannot be created, filename "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" is too long for tar file format
tar-based phar "%sbignames.4.tar" cannot be created, filename "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" is too long for tar file format
