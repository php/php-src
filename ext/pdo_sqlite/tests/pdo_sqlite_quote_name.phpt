--TEST--
PDO SQLite quote name
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php
$pdo = new PDO("sqlite::memory:");

echo $pdo->quoteName('id') . "\n";
echo $pdo->quoteName('order') . "\n";
echo $pdo->quoteName('') . "\n";
echo $pdo->quoteName('x"y') . "\n";
?>
--EXPECT--
"id"
"order"
""
"x""y"
