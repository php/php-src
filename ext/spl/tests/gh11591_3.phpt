--TEST--
GH-11591 (AppendIterator skips probed-empty generators after array mutations)
--FILE--
<?php
function emptyGenerator(): Generator {
    if (false) {
        yield;
    }
}

function values(AppendIterator $iterator): void {
    try {
        var_dump(iterator_to_array($iterator, false));
    } catch (Throwable $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
}

echo "reinsert same key\n";
$generator = emptyGenerator();
$iterator = new AppendIterator();
$iterator->append($generator);
$outer = $iterator->getArrayIterator();
$outer->offsetUnset(0);
$outer->offsetSet(0, $generator);
values($iterator);

echo "reinsert other key\n";
$generator = emptyGenerator();
$iterator = new AppendIterator();
$iterator->append($generator);
$outer = $iterator->getArrayIterator();
$outer->offsetUnset(0);
$outer->offsetSet(1, $generator);
values($iterator);

echo "replace with non-empty generator\n";
$generator = emptyGenerator();
$iterator = new AppendIterator();
$iterator->append($generator);
$outer = $iterator->getArrayIterator();
$outer->offsetUnset(0);
$outer->offsetSet(0, (function () { yield 'NEW'; })());
values($iterator);

echo "empty appended after valid entry\n";
$iterator = new AppendIterator();
$iterator->append(new ArrayIterator(['A']));
$iterator->append(emptyGenerator());
values($iterator);
values($iterator);

echo "empty generator behind wrapper\n";
$iterator = new AppendIterator();
$iterator->append(new IteratorIterator(emptyGenerator()));
$iterator->append(new ArrayIterator(['A']));
values($iterator);
values($iterator);
?>
--EXPECT--
reinsert same key
array(0) {
}
reinsert other key
array(0) {
}
replace with non-empty generator
array(1) {
  [0]=>
  string(3) "NEW"
}
empty appended after valid entry
array(1) {
  [0]=>
  string(1) "A"
}
array(1) {
  [0]=>
  string(1) "A"
}
empty generator behind wrapper
array(1) {
  [0]=>
  string(1) "A"
}
array(1) {
  [0]=>
  string(1) "A"
}
