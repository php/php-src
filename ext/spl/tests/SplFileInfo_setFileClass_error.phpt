--TEST--
SplFileInfo::setFileClass() throws exception for invalid class
--FILE--
<?php

$info = new SplFileInfo(__FILE__);

try {
    $info->setFileClass('stdClass');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
SplFileInfo::setFileClass(): Argument #1 ($class) must be a class name derived from SplFileObject, stdClass given
