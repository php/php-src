<?php

/** @generate-class-entries */

class SplFixedArray implements IteratorAggregate, ArrayAccess, Countable, JsonSerializable
{
    public function __construct(int $size = 0) {}

    /** @tentative-return-type */
    public function __wakeup(): void {}

    public function __serialize(): array {}

    public function __unserialize(array $data): void {}

    /** @tentative-return-type */
    public function count(): int {}

    /** @tentative-return-type */
    public function toArray(): array {}

    /** @tentative-return-type */
    public static function fromArray(array $array, bool $preserveKeys = true): SplFixedArray {}

    /** @tentative-return-type */
    public function getSize(): int {}

    /** @return bool */
    public function setSize(int $size) {} // TODO make return type void

    /**
     * @param int $index
     * @tentative-return-type
     */
    public function offsetExists($index): bool {}

    /**
     * @param int $index
     * @tentative-return-type
     */
    public function offsetGet($index): mixed {}

    /**
     * @param int $index
     * @tentative-return-type
     */
    public function offsetSet($index, mixed $value): void {}

    /**
     * @param int $index
     * @tentative-return-type
     */
    public function offsetUnset($index): void {}

    public function getIterator(): Iterator {}

    public function jsonSerialize(): array {}
}
