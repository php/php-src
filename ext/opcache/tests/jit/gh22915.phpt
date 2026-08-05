--TEST--
GH-22915: compiled exit clobbers registers before saving
--EXTENSIONS--
opcache
--INI--
opcache.jit_max_side_traces=0
opcache.jit_blacklist_side_trace=0
--ENV--
F=iter
--FILE--
<?php

final class It implements Iterator {
    public readonly array $values;
    public int $position = 0;
    public function __construct(array $values) {
        $this->values = $values;
    }

    public function rewind(): void {}

    public function valid(): bool {
        return $this->position === 0;
    }

    public function current(): mixed {
        if (!isset($this->values[$this->position])) {
            throw new Exception();
        }

        return $this->values[$this->position];
    }

    public function key(): mixed {
        return $this->position;
    }

    public function next(): void {
        $this->position++;
    }
}

function iter(It $it) {
    foreach ($it as $value) {
        var_dump($value);
        if (!$value instanceof stdClass) {
            continue;
        }
    }
}

echo "# First run\n";
for ($i = 0; $i < 5; $i++) {
    getenv('F')(new It([getenv('F')]));                // non-immutable, packed array
}

echo "# Second run\n";
for ($i = 0; $i < 5; $i++) {
    getenv('F')(new It([getenv('F'), 'map' => true])); // non-immutable, map, triggers exit
}

?>
--EXPECT--
# First run
string(4) "iter"
string(4) "iter"
string(4) "iter"
string(4) "iter"
string(4) "iter"
# Second run
string(4) "iter"
string(4) "iter"
string(4) "iter"
string(4) "iter"
string(4) "iter"
