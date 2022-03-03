--TEST--
SQLite3 - memory leak on SQLite3Result and SQLite3Stmt
--EXTENSIONS--
sqlite3
--FILE--
<?php

function test(&$x) {
    $class = new SQLite3(':memory:');
    $x = $class->prepare('SELECT 1');
}

test($foo);

echo "done\n";

?>
--EXPECT--
done
