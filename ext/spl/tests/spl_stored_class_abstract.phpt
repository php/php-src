--TEST--
SPL rejects abstract classes when instantiating a stored class
--FILE--
<?php
abstract class AbstractIterator extends ArrayIterator {}
abstract class AbstractFileObject extends SplFileObject {}
abstract class AbstractFileInfo extends SplFileInfo {}

function test(string $label, callable $fn): void {
    try {
        $fn();
        echo $label, ": no exception", PHP_EOL;
    } catch (Error $e) {
        echo $label, ": ", $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}

test("ArrayObject constructor", fn() => (new ArrayObject([1], 0, "AbstractIterator"))->getIterator());
test("setIteratorClass()", function () {
    $ao = new ArrayObject([1]);
    $ao->setIteratorClass("AbstractIterator");
    $ao->getIterator();
});

$info = new SplFileInfo(__FILE__);
test("openFile()", function () use ($info) {
    $info->setFileClass("AbstractFileObject");
    $info->openFile();
});
test("getFileInfo() argument", fn() => $info->getFileInfo("AbstractFileInfo"));
test("getPathInfo() argument", fn() => $info->getPathInfo("AbstractFileInfo"));
test("setInfoClass()", function () use ($info) {
    $info->setInfoClass("AbstractFileInfo");
    $info->getFileInfo();
});

$it = new FilesystemIterator(__DIR__, FilesystemIterator::CURRENT_AS_FILEINFO);
$it->setInfoClass("AbstractFileInfo");
test("FilesystemIterator::current()", fn() => $it->current());
test("FilesystemIterator foreach", function () use ($it) {
    foreach ($it as $file) {
        break;
    }
});
?>
--EXPECT--
ArrayObject constructor: Error: Cannot instantiate abstract class AbstractIterator
setIteratorClass(): Error: Cannot instantiate abstract class AbstractIterator
openFile(): Error: Cannot instantiate abstract class AbstractFileObject
getFileInfo() argument: Error: Cannot instantiate abstract class AbstractFileInfo
getPathInfo() argument: Error: Cannot instantiate abstract class AbstractFileInfo
setInfoClass(): Error: Cannot instantiate abstract class AbstractFileInfo
FilesystemIterator::current(): Error: Cannot instantiate abstract class AbstractFileInfo
FilesystemIterator foreach: Error: Cannot instantiate abstract class AbstractFileInfo
