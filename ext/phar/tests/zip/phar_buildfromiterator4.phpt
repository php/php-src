--TEST--
Phar::buildFromIterator() iterator, 1 file passed in zip-based
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
chdir(__DIR__);
$phar = new Phar(__DIR__ . '/buildfromiterator4.phar.zip');
var_dump($phar->buildFromIterator(new myIterator(array('a' => basename(__FILE__, 'php') . 'phpt'))));
var_dump($phar->isFileFormat(Phar::ZIP));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromiterator4.phar.zip');
?>
--EXPECTF--
rewind
valid
current
key
next
valid
array(1) {
  ["a"]=>
  string(%d) "%sphar_buildfromiterator4.phpt"
}
bool(true)
