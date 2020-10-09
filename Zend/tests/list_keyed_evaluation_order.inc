<?php declare(strict_types=1);

// Observer objects for the Zend/tests/list_keyed_evaluation_order.* tests

class StringCapable
{
    private $name;
    public function __construct(string $name) {
        $this->name = $name;
    }

    public function __toString(): string {
        echo "$this->name evaluated.", PHP_EOL;
        return $this->name;
    }
}

class Indexable implements ArrayAccess
{
    private $array;
    public function __construct(array $array) {
        $this->array = $array;
    }

    public function offsetExists($offset): bool {
        echo "Existence of offset $offset checked for.", PHP_EOL;
        return isset($this->array[$offset]);
    }

    public function offsetUnset($offset): void {
        unset($this->array[$offset]);
        echo "Offset $offset removed.", PHP_EOL;
    }

    public function offsetGet($offset) {
        echo "Offset $offset retrieved.", PHP_EOL;
        return $this->array[$offset];
    }

    public function offsetSet($offset, $value): void {
        $this->array[$offset] = $value;
        echo "Offset $offset set to $value.", PHP_EOL;
    }
}

class IndexableRetrievable
{
    private $label;
    private $indexable;

    public function __construct(string $label, Indexable $indexable) {
        $this->label = $label;
        $this->indexable = $indexable;
    }

    public function getIndexable(): Indexable {
        echo "Indexable $this->label retrieved.", PHP_EOL;
        return $this->indexable;
    }
}
