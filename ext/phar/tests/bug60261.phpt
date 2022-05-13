--TEST--
Bug #60261 (phar dos null pointer)
--EXTENSIONS--
phar
--FILE--
<?php

try {
    $nx = new Phar();
    $nx->getLinkTarget();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Phar::__construct() expects at least 1 argument, 0 given
