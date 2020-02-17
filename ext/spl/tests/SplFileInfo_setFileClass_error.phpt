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
SplFileInfo::setFileClass() expects argument #1 ($class_name) to be a class name derived from SplFileObject, 'stdClass' given
