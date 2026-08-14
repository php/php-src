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

echo "replace with unstarted generator\n";
$generator = emptyGenerator();
$iterator = new AppendIterator();
$iterator->append($generator);
$outer = $iterator->getArrayIterator();
$outer->offsetUnset(0);
$outer->offsetSet(0, emptyGenerator());
values($iterator);
?>
--EXPECT--
reinsert same key
array(0) {
}
reinsert other key
array(0) {
}
replace with unstarted generator
array(0) {
}
