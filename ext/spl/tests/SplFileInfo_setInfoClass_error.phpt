--TEST--
SplFileInfo::setInfoClass() throws exception for invalid class
--FILE--
<?php

$info = new SplFileInfo(__FILE__);

try {
    $info->setInfoClass('stdClass');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
SplFileInfo::setInfoClass(): Argument #1 ($class) must be a class name derived from SplFileInfo, stdClass given
