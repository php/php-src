--TEST--
Phar::buildFromIterator() iterator, key is int zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
class myIterator implements Iterator
{
    var $a;
    function __construct(array $a)
    {
        $this->a = $a;
    }
    function next() {
        echo "next\n";
        return next($this->a);
    }
    function current() {
        echo "current\n";
        return current($this->a);
    }
    function key() {
        echo "key\n";
        return key($this->a);
    }
    function valid() {
        echo "valid\n";
        return current($this->a);
    }
    function rewind() {
        echo "rewind\n";
        return reset($this->a);
    }
}
try {
    chdir(__DIR__);
    $phar = new Phar(__DIR__ . '/buildfromiterator.phar.zip');
    var_dump($phar->buildFromIterator(new myIterator(array(basename(__FILE__, 'php') . 'phpt'))));
} catch (Exception $e) {
    var_dump(get_class($e));
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromiterator.phar.zip');
__HALT_COMPILER();
?>
--EXPECTF--
rewind
valid
current
key
%s(24) "UnexpectedValueException"
Iterator myIterator returned an invalid key (must return a string)
