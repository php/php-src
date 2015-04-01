--TEST--
Testing PDORow and PDOStatement instances with Reflection
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

$instance = new reflectionclass('pdostatement');
$x = $instance->newInstance();

if ($x instanceof pdostatement) {
    echo "Ok".PHP_EOL;
}
else {
    echo "Failed to create instance of pdostatment";
}

try {
    $instance = new reflectionclass('pdorow');
    $x = $instance->newInstance();
    echo "Failed to throw exception: ".var_export($x, true);
}
catch(PDOException $pe) {
    if ($pe->getMessage() != "You may not create a PDORow manually") {
        echo "PDOException has wrong message.";
    }
    else {
        echo "Ok".PHP_EOL;
    }
}

?>
--EXPECTF--
Ok
Ok
