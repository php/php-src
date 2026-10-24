--TEST--
SQLite3Stmt::close() after a fatal error unwound out of a callback mid-step
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled, memory_limit is not enforced");
?>
--INI--
memory_limit=8M
--FILE--
<?php
register_shutdown_function(function () {
    global $stmt;
    /* The statement is no longer executing, so it must still be closable
       even though the fatal unwound out of the middle of a step. */
    var_dump($stmt->close());
    echo "shutdown done\n";
});

$db = new SQLite3(':memory:');
$db->createFunction('bail', function () {
    str_repeat('x', 100000000);
});
$stmt = $db->prepare('SELECT bail()');
$stmt->execute();
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s
bool(true)
shutdown done
