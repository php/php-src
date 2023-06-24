<?php

/** @generate-class-entries */

// This being "SeqCollection" but the keyword being "Seq" is going to be confusing, I expect.
// This sounds like what namespaces are good for.
interface SeqCollection
{
    // IMO, this method should return $this so it's more conveniently chainable.
	public function add(mixed $value) : void;

    // Modifies in place.  Equivalent of -=.
    public function remove(mixed $value) : static;

    public function has(mixed $value) : bool;

    public function get(int $index) : mixed;

    // Returns new object with the value added.  Equivalent of +.
    public function with(mixed $value) : static;

    // Returns new object with the value removed if it was there.  Equivalent of -
    public function without(mixed $value) : static;

    // returns self.  Throws OutOfBoundsException if $index is not yet defined.
    public function set(int $index, mixed $value): static;

    // This should really be static $other, but the language doesn't allow that.
    // Equivalent of + static.
    public function concat(self $other): static;

    // True if both seqs have the same values in the same order.
    public function equals(self $other): bool;

    // $fn is callable(mixed $val)
    // The return type is $targetType, but that can't be expressed statically.
    public function map(callable $fn, string $targetType): SeqCollection;

}

interface DictCollection
{
	public function add(mixed $key, mixed $value) : void;

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
    public function concat(self $other): static;

    // True if both dicts have the same key/values in the same order.
    public function equals(self $other): bool;

    // $fn is callable(mixed $val, mixed $key)
    // The return type is $targetType, but that can't be expressed statically.
    public function map(callable $fn, string $targetType): SeqCollection;
}
