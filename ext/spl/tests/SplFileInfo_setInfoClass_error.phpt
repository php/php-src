--TEST--
SplFileInfo::setInfoClass() throws exception for invalid class
--FILE--
<?php

$info = new SplFileInfo(__FILE__);

try {
    $info->setInfoClass('stdClass');
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
SplFileInfo::setInfoClass() expects parameter 1 to be a class name derived from SplFileInfo, 'stdClass' given
