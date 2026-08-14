--TEST--
GH-11591 (AppendIterator preserves other iterator semantics)
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
        echo $e->getMessage(), "\n";
    }
}

echo "duplicate\n";
$generator = emptyGenerator();
$iterator = new AppendIterator();
$iterator->append($generator);
try {
    $iterator->append($generator);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "replacement\n";
$iterator = new AppendIterator();
$iterator->append(emptyGenerator());
$iterator->getArrayIterator()[0] = new ArrayIterator(['R']);
values($iterator);

echo "ordinary\n";
$ordinary = new class implements Iterator {
    public int $rewinds = 0;
    public function rewind(): void { $this->rewinds++; }
    public function valid(): bool { return false; }
    public function current(): mixed { return null; }
    public function key(): mixed { return null; }
    public function next(): void {}
};
$iterator = new AppendIterator();
$iterator->append($ordinary);
echo $ordinary->rewinds, "\n";
values($iterator);
echo $ordinary->rewinds, "\n";
values($iterator);
echo $ordinary->rewinds, "\n";

echo "nonempty\n";
$generator = (function () { yield 'G'; })();
$iterator = new AppendIterator();
$iterator->append($generator);
values($iterator);
values($iterator);

echo "moved\n";
$generator = emptyGenerator();
$iterator = new AppendIterator();
$iterator->append($generator);
$array = $iterator->getArrayIterator();
unset($array[0]);
$array[] = $generator;
values($iterator);

echo "append during traversal\n";
$iterator = new AppendIterator();
$iterator->append(new ArrayIterator(['A']));
$added = false;
foreach ($iterator as $value) {
    echo $value;
    if (!$added) {
        $iterator->append(emptyGenerator());
        $iterator->append(new ArrayIterator(['B']));
        $added = true;
    }
}
echo "\n";

echo "externally closed\n";
$generator = emptyGenerator();
$generator->valid();
$iterator = new AppendIterator();
try {
    $iterator->append($generator);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

echo "weak lifetime\n";
$generator = emptyGenerator();
$weak = WeakReference::create($generator);
$iterator = new AppendIterator();
$iterator->append($generator);
unset($iterator->getArrayIterator()[0], $generator);
gc_collect_cycles();
var_dump($weak->get() === null);

echo "reentrant unset\n";
$iterator = new AppendIterator();
$generator = (function () use (&$iterator): Generator {
    unset($iterator->getArrayIterator()[0]);
    if (false) {
        yield;
    }
})();
$iterator->append($generator);
values($iterator);

echo "reentrant move\n";
$iterator = new AppendIterator();
$generator = (function () use (&$iterator, &$generator): Generator {
    $array = $iterator->getArrayIterator();
    unset($array[0]);
    $array[1] = $generator;
    if (false) {
        yield;
    }
})();
$iterator->append($generator);
values($iterator);
?>
--EXPECT--
duplicate
Cannot traverse an already closed generator
replacement
array(1) {
  [0]=>
  string(1) "R"
}
ordinary
1
array(0) {
}
2
array(0) {
}
3
nonempty
array(1) {
  [0]=>
  string(1) "G"
}
Cannot traverse an already closed generator
moved
array(0) {
}
append during traversal
AB
externally closed
Cannot traverse an already closed generator
weak lifetime
bool(true)
reentrant unset
array(0) {
}
reentrant move
array(0) {
}
