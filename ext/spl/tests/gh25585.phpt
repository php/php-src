--TEST--
GH-25585 (SPL objects of a stored class skip constant evaluation)
--FILE--
<?php
const VALUE = 42;

class MyIterator extends ArrayIterator {
    public $var = VALUE;
}
class SetIterator extends ArrayIterator {
    public $var = VALUE;
}
class UndefIterator extends ArrayIterator {
    public $var = UNDEF_CONST;
}

class MyFileObject extends SplFileObject {
    public $var = VALUE;
}
class MyFileInfo extends SplFileInfo {
    public $var = VALUE;
}
class MyPathInfo extends SplFileInfo {
    public $var = VALUE;
}
class MyDirInfo extends SplFileInfo {
    public $var = VALUE;
}
class UndefFileInfo extends SplFileInfo {
    public $var = UNDEF_CONST;
}

$ao = new ArrayObject(["a" => 1], 0, "MyIterator");
foreach ($ao as $v) {}
echo "ArrayObject iterator class: "; var_dump($ao->getIterator()->var);

$ao = new ArrayObject([1]);
$ao->setIteratorClass("SetIterator");
echo "setIteratorClass(): "; var_dump($ao->getIterator()->var);

$ao = new ArrayObject([1], 0, "UndefIterator");
try {
    $ao->getIterator();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

$info = new SplFileInfo(__FILE__);
$info->setFileClass("MyFileObject");
echo "openFile(): "; var_dump($info->openFile()->var);
$info->setInfoClass("MyFileInfo");
echo "getFileInfo(): "; var_dump($info->getFileInfo()->var);
echo "getPathInfo(): "; var_dump($info->getPathInfo("MyPathInfo")->var);

$it = new FilesystemIterator(__DIR__, FilesystemIterator::CURRENT_AS_FILEINFO);
$it->setInfoClass("MyDirInfo");
foreach ($it as $file) {
    echo "FilesystemIterator: "; var_dump($file->var);
    break;
}

$it = new FilesystemIterator(__DIR__, FilesystemIterator::CURRENT_AS_FILEINFO);
$it->setInfoClass("UndefFileInfo");
try {
    $it->current();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

$it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator(__DIR__, FilesystemIterator::SKIP_DOTS));
$it->getInnerIterator()->setInfoClass("UndefFileInfo");
for ($i = 0; $i < 2; $i++) {
    try {
        $it->current();
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
ArrayObject iterator class: int(42)
setIteratorClass(): int(42)
Error: Undefined constant "UNDEF_CONST"
openFile(): int(42)
getFileInfo(): int(42)
getPathInfo(): int(42)
FilesystemIterator: int(42)
Error: Undefined constant "UNDEF_CONST"
Error: Undefined constant "UNDEF_CONST"
Error: Undefined constant "UNDEF_CONST"
