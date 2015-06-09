--TEST--
SplFileInfo::setInfoClass() expects SplFileInfo or child class
--FILE--
<?php

class MyInfoObject extends SplFileInfo {}

$info = new SplFileInfo(__FILE__);

$info->setInfoClass('MyInfoObject');
echo get_class($info->getFileInfo()), "\n";
echo get_class($info->getPathInfo()), "\n";

$info->setInfoClass('SplFileInfo');
echo get_class($info->getFileInfo()), "\n";
echo get_class($info->getPathInfo()), "\n";

?>
--EXPECT--
MyInfoObject
MyInfoObject
SplFileInfo
SplFileInfo
