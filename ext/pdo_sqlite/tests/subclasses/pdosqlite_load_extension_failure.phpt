--TEST--
PdoSqlite load extension
--EXTENSIONS--
pdo
--FILE--
<?php

$db = Pdo::connect('sqlite::memory:');

if (!$db instanceof PdoSqlite) {
    echo "Wrong class type. Should be PdoSqlite but is [" .get_class($db) . "\n";
}


try {
    echo "Loading non-existent file.\n";
    $result = $db->loadExtension("/this/does/not_exist");
    echo "Failed to throw exception";
}
catch (PDOException $pdoException) {
    echo $pdoException->getMessage() . "\n";
}



try {
    echo "Loading invalid file.\n";
    $result = $db->loadExtension(__FILE__);
    echo "Failed to throw exception";
}
catch (PDOException $pdoException) {
    echo $pdoException->getMessage() . "\n";
}


echo "Fin.";
?>
--EXPECTF--
Loading non-existent file.
Unable to load extension '/this/does/not_exist'
Loading invalid file.
Unable to load extension '%s: No such file or directory'
Fin.
