--TEST--
SPL: Inheritance of SplFileObject, SplFileInfo misses constructor code.
--FILE--
<?php

class TestFileObject extends \SplFileObject {
    public function __construct($file_name, $open_mode = 'r', $use_include_path = false, $context = null) {
        if (is_resource($context)) {
            parent::__construct($file_name, $open_mode, $use_include_path, $context);
        }
        else {
            parent::__construct($file_name, $open_mode, $use_include_path);
        }
    }
}

class TestFileInfo extends \SplFileInfo {
    function __construct($file_name) {
        parent::__construct($file_name);
    } 
}

$filename = dirname(__FILE__) . '/foo.temp';

//Test that the fileInfo to fileObject works
$fileInfo = new SplFileInfo($filename);
$fileInfo->setFileClass('TestFileObject');
$fileInstance = $fileInfo->openFile('w+');

if (!($fileInstance instanceof TestFileObject)) {
    echo "Incorrect type of fileInstance ".get_class($fileInstance).PHP_EOL;
}


$filename2 = dirname(__FILE__) . '/foo2.temp';
//Test that the fileObject to fileInfo works
$fileObject = new SplFileObject($filename2, 'w+');
$fileObject->setInfoClass('TestFileInfo');
$fileInfo = $fileObject->getFileInfo();

if (!($fileInfo instanceof TestFileInfo)) {
echo "Incorrect type of fileInfo ".get_class($fileInstance).PHP_EOL;
}

$fsIterator = new FilesystemIterator( dirname(__FILE__), FilesystemIterator::SKIP_DOTS);
$fsIterator->setInfoClass('TestFileInfo');
$fsIterator->setFileClass('TestFileObject');
foreach ($fsIterator as $fsEntry) {
    if (!($fsEntry instanceof TestFileInfo)) {
        echo "Incorrect type of fileInfo ".get_class($fsEntry).PHP_EOL;
    }
}

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/foo.temp');
unlink(dirname(__FILE__) . '/foo2.temp');
?>
--EXPECT--

===DONE===
