--TEST--
GH-11591 (AppendIterator with generators that yield nothing)
--FILE--
<?php
function emptyGenerator(): Generator {
    if (false) {
        yield;
    }
}

function values(AppendIterator $iterator): void {
    var_dump(iterator_to_array($iterator, false));
}

echo "sole\n";
$iterator = new AppendIterator();
$iterator->append(emptyGenerator());
values($iterator);
values($iterator);

echo "leading\n";
$iterator = new AppendIterator();
$iterator->append(emptyGenerator());
$iterator->append(new ArrayIterator(['A']));
values($iterator);
values($iterator);

echo "distinct\n";
$iterator = new AppendIterator();
$iterator->append(emptyGenerator());
$iterator->append(emptyGenerator());
values($iterator);
?>
--EXPECT--
sole
array(0) {
}
array(0) {
}
leading
array(1) {
  [0]=>
  string(1) "A"
}
array(1) {
  [0]=>
  string(1) "A"
}
distinct
array(0) {
}
