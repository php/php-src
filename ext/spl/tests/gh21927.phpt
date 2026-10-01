--TEST--
GH-21927: Use-after-free of self-freeing MultipleIterator children
--FILE--
<?php

class DetachOnRewind implements Iterator {
    public function __construct(private MultipleIterator $parent) {}
    public function rewind(): void {
        $this->parent->detachIterator($this);
        echo "rewind: still alive\n";
    }
    public function next(): void {}
    public function current(): mixed { return 0; }
    public function key(): mixed { return 0; }
    public function valid(): bool { return false; }
}

class DetachOnNext implements Iterator {
    public function __construct(private MultipleIterator $parent) {}
    public function rewind(): void {}
    public function next(): void {
        $this->parent->detachIterator($this);
        echo "next: still alive\n";
    }
    public function current(): mixed { return 0; }
    public function key(): mixed { return 0; }
    public function valid(): bool { return true; }
}

class DetachOnValid implements Iterator {
    public function __construct(private MultipleIterator $parent) {}
    public function rewind(): void {}
    public function next(): void {}
    public function current(): mixed { return 0; }
    public function key(): mixed { return 0; }
    public function valid(): bool {
        $this->parent->detachIterator($this);
        echo "valid: still alive\n";
        return true;
    }
}

class DetachOnCurrent implements Iterator {
    public function __construct(private MultipleIterator $parent) {}
    public function rewind(): void {}
    public function next(): void {}
    public function current(): mixed {
        $this->parent->detachIterator($this);
        echo "current: still alive\n";
        return 'C';
    }
    public function key(): mixed { return 'k'; }
    public function valid(): bool { return true; }
}

class DetachOnKey implements Iterator {
    public function __construct(private MultipleIterator $parent) {}
    public function rewind(): void {}
    public function next(): void {}
    public function current(): mixed { return 'C'; }
    public function key(): mixed {
        $this->parent->detachIterator($this);
        echo "key: still alive\n";
        return 'K';
    }
    public function valid(): bool { return true; }
}

echo "-- detach inside rewind --\n";
$mi = new MultipleIterator();
$mi->attachIterator(new DetachOnRewind($mi));
$mi->rewind();
var_dump($mi->countIterators());

echo "-- detach inside next --\n";
$mi = new MultipleIterator();
$mi->attachIterator(new DetachOnNext($mi));
$mi->rewind();
$mi->next();
var_dump($mi->countIterators());

echo "-- detach inside valid --\n";
$mi = new MultipleIterator();
$mi->attachIterator(new DetachOnValid($mi));
var_dump($mi->valid());
var_dump($mi->countIterators());

echo "-- detach inside current (numeric keys) --\n";
$mi = new MultipleIterator();
$mi->attachIterator(new DetachOnCurrent($mi));
var_dump($mi->current());
var_dump($mi->countIterators());

echo "-- detach inside key (numeric keys) --\n";
$mi = new MultipleIterator();
$mi->attachIterator(new DetachOnKey($mi));
var_dump($mi->key());
var_dump($mi->countIterators());

echo "-- detach inside current (assoc keys, refcounted inf) --\n";
$mi = new MultipleIterator(MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_ASSOC);
$mi->attachIterator(new DetachOnCurrent($mi), 'cur_info_string');
var_dump($mi->current());
var_dump($mi->countIterators());

echo "-- detach inside key (assoc keys, refcounted inf) --\n";
$mi = new MultipleIterator(MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_ASSOC);
$mi->attachIterator(new DetachOnKey($mi), 'key_info_string');
var_dump($mi->key());
var_dump($mi->countIterators());

?>
--EXPECT--
-- detach inside rewind --
rewind: still alive
int(0)
-- detach inside next --
next: still alive
int(0)
-- detach inside valid --
valid: still alive
bool(true)
int(0)
-- detach inside current (numeric keys) --
current: still alive
array(1) {
  [0]=>
  string(1) "C"
}
int(0)
-- detach inside key (numeric keys) --
key: still alive
array(1) {
  [0]=>
  string(1) "K"
}
int(0)
-- detach inside current (assoc keys, refcounted inf) --
current: still alive
array(1) {
  ["cur_info_string"]=>
  string(1) "C"
}
int(0)
-- detach inside key (assoc keys, refcounted inf) --
key: still alive
array(1) {
  ["key_info_string"]=>
  string(1) "K"
}
int(0)
