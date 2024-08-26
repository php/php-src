<?php

/** @generate-class-entries */

// This being "SeqCollection" but the keyword being "Seq" is going to be confusing, I expect.
// This sounds like what namespaces are good for.
interface SeqCollection
{
    public function add(mixed $value) : static;

    public function remove(int $index) : static;

    public function has(int $index) : bool;

    public function get(int $index) : mixed;

    // Returns new object with the value added.  Equivalent of +.
    public function with(mixed $value) : static;

    // Returns new object with the value removed if it was there.  Equivalent of -
    public function without(int $index) : static;

    // returns self.  Throws OutOfBoundsException if $index is not yet defined.
    public function set(int $index, mixed $value): static;

    // This should really be static $other, but the language doesn't allow that.
    // Equivalent of + static.
    public function concat(object $other): static;

    // True if both seqs have the same values in the same order.
    public function equals(object $other): bool;

    // $fn is callable(mixed $val)
    // The return type is $targetType, but that can't be expressed statically.
    public function map(callable $fn, string $targetType): object;
}

interface DictCollection
{
    public function add(mixed $key, mixed $value) : static;

    public function remove(mixed $key) : static;

    public function has(mixed $key) : bool;

    public function get(mixed $key) : mixed;

    // Returns new object with the value added.
    public function with(mixed $key, mixed $value) : static;

    // Returns new object with the value removed if it was there.
    public function without(mixed $key) : static;

    // returns self.  Overwrites existing value if already set.
    public function set(mixed $key, mixed $value): static;

    // This should really be static $other, but the language doesn't allow that.
    // Equivalent of + static.
    public function concat(object $other): static;

    // True if both dicts have the same key/values in the same order.
    public function equals(object $other): bool;

    // $fn is callable(mixed $val, mixed $key)
    // The return type is $targetType, but that can't be expressed statically.
    public function map(callable $fn, string $targetType): object;
}
