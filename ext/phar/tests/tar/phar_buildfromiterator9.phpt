--TEST--
Phar::buildFromIterator() iterator, 1 file resource passed in tar-based
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
	$phar = new Phar(__DIR__ . '/buildfromiterator.phar.tar');
	var_dump($phar->buildFromIterator(new myIterator(array('a' => $a = fopen(basename(__FILE__, 'php') . 'phpt', 'r')))));
	fclose($a);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromiterator.phar.tar');
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
===DONE===
