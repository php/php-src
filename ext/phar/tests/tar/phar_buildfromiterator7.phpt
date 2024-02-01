--TEST--
Phar::buildFromIterator() iterator, file can't be opened tar-based
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
    var_dump($phar->buildFromIterator(new myIterator(array('a' => basename(__FILE__, 'php') . '/oopsie/there.phpt'))));
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
Iterator myIterator returned a file that could not be opened "phar_buildfromiterator7./oopsie/there.phpt"
