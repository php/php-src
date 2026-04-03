--TEST--
Concrete example of using AT
--CREDITS--
Levi Morrison
--FILE--
<?php declare(strict_types=1);

namespace Sequence;

// No null. This is probably going to be painful, but let's try it.
interface Sequence<Item : object|array|string|float|int|bool>
{
    function next(): ?Item;

    /**
     * @param callable(Item, Item): Item $f
     * @return ?Item
     */
    function reduce(callable $f): ?Item;
}

final class StringTablePair
{
    public function __construct(
        public readonly string $string,
        public readonly int $id,
    ) {}
}

final class StringTableSequence implements Sequence<StringTablePair>
{
    private array $strings;

    public function __construct(StringTable $string_table) {
        $this->strings = $string_table->to_assoc_array();
    }

    function next(): ?StringTablePair
    {
        $key = \array_key_first($this->strings);
        if (!isset($key)) {
            return null;
        }
        $value = \array_shift($this->strings);
        return new StringTablePair($key, $value);
    }

    /**
     * @param callable(Item, Item): Item $f
     * @return ?Item
     */
    function reduce(callable $f): ?StringTablePair
    {
        $reduction = $this->next();
        if (!isset($reduction)) {
            return null;
        }

        while (($next = $this->next()) !== null) {
            $reduction = $f($reduction, $next);
        }
        return $reduction;
    }
}

final class StringTable
{
    private array $strings = ["" => 0];

    public function __construct() {}

    public function offsetGet(string $offset): int
    {
        return $this->strings[$offset] ?? throw new \Exception();
    }

    public function offsetExists(string $offset): bool
    {
        return \isset($this->strings[$offset]);
    }

    public function intern(string $str): int
    {
        return $this->strings[$str]
            ?? ($this->strings[$str] = \count($this->strings));
    }

    public function to_sequence(): StringTableSequence
    {
        return new StringTableSequence($this);
    }

    public function to_assoc_array(): array {
        return $this->strings;
    }
}

?>
--EXPECTF--
Fatal error: Generic type cannot be part of a union type in %s on line %d
