--TEST--
Failure when creating persistent connection
--EXTENSIONS--
pdo_mysql
--FILE--
<?php
try {
    $pdo = new PDO('mysql:host=localhost', 'invalid_user', 'invalid_password', [
        PDO::ATTR_PERSISTENT => true,
    ]);
} catch (PDOException $e) {
    echo "Caught\n";
}
?>
--EXPECT--
Caught
