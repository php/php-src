--TEST--
SplFileInfo::setFileClass() throws exception for invalid class
--FILE--
<?php

$info = new SplFileInfo(__FILE__);

try {
    $info->setFileClass('stdClass');
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
SplFileInfo::setFileClass() expects parameter 1 to be a class name derived from SplFileObject, 'stdClass' given
