--TEST--
Trying instantiate a PDORow object manually
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

try {
    $instance = new PDORow;
    echo "Failed to throw exception.".var_export($instance, true);
}
catch(PDOException $pe) {
        if ($pe->getMessage() != "You may not create a PDORow manually") {
            echo "PDOException has wrong message.";
        }
        else {
            echo "Ok".PHP_EOL;
        }
}
catch(\Exception $e) {
    echo "Failed to ";
}

?>
--EXPECTF--
Ok
