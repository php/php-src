--TEST--
Phar::buildFromIterator() iterator, key is int tar-based
--EXTENSIONS--
phar
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
    function next(): void {
        echo "next\n";
        next($this->a);
    }
    function current(): mixed {
        echo "current\n";
        return current($this->a);
    }
    function key(): mixed {
        echo "key\n";
        return key($this->a);
    }
    function valid(): bool {
        echo "valid\n";
        return current($this->a);
    }
    function rewind(): void {
        echo "rewind\n";
        reset($this->a);
    }
}
try {
    chdir(__DIR__);
    $phar = new Phar(__DIR__ . '/buildfromiterator.phar.tar');
    var_dump($phar->buildFromIterator(new myIterator(array(basename(__FILE__, 'php') . 'phpt'))));
} catch (Exception $e) {
    var_dump(get_class($e));
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
rewind
valid
current
key
%s(24) "UnexpectedValueException"
Iterator myIterator returned an invalid key (must return a string)
