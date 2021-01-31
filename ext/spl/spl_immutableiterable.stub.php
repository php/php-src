<?php

/** @generate-class-entries */

final class ImmutableIterable implements IteratorAggregate, Countable, JsonSerializable
{
    public function __construct(iterable $iterator) {}
    public function getIterator(): InternalIterator {}
    public function count(): int {}
    public static function fromPairs(iterable $pairs): ImmutableIterable {}
    public function toPairs(): array {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): ImmutableIterable {}

    public function keys(): array {}
    public function values(): array {}
    public function keyAt(int $offset): mixed {}
    public function valueAt(int $offset): mixed {}

    public function jsonSerialize(): array {}
}
