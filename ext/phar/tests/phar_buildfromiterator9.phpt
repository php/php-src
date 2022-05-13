--TEST--
Phar::buildFromIterator() iterator, 1 file resource passed in
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
        return (bool) current($this->a);
    }
    function rewind(): void {
        echo "rewind\n";
        reset($this->a);
    }
}
try {
    chdir(__DIR__);
    $phar = new Phar(__DIR__ . '/buildfromiterator9.phar');
    var_dump($phar->buildFromIterator(new myIterator(array('a' => $a = fopen(basename(__FILE__, 'php') . 'phpt', 'r')))));
    fclose($a);
} catch (Exception $e) {
    var_dump(get_class($e));
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromiterator9.phar');
__HALT_COMPILER();
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
  string(%d) "[stream]"
}
