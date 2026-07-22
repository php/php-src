--TEST--
GH-22854 (Assertion failure at link_errno_read in ext/mysqli/mysqli_prop.c)
--EXTENSIONS--
mysqli
--FILE--
<?php
mysqli_report(MYSQLI_REPORT_OFF);

/* An initialized-but-not-connected object: after a failed connect(),
 * the internal MYSQL* handle is NULL while the resource status stays
 * INITIALIZED. Reading the errno/error properties used to dereference the
 * NULL handle and abort on ZEND_ASSERT(p). */
$mysqli = new mysqli();
@$mysqli->connect('localhost', 'user', 'pass', 'db', 0, '/mysqli/gh22854/does/not/exist.sock');

try {
    var_dump($mysqli->errno);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump($mysqli->error);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
mysqli object is not fully initialized
mysqli object is not fully initialized
done
