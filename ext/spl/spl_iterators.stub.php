<?php

/** @generate-class-entries */

class EmptyIterator implements Iterator
{
    /** @tentative-return-type */
    public function current(): never {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function key(): never {}

    /** @tentative-return-type */
    public function valid(): false {}

    /** @tentative-return-type */
    public function rewind(): void {}
}

class CallbackFilterIterator extends FilterIterator
{
    public function __construct(Iterator $iterator, callable $callback) {}

    /** @tentative-return-type */
    public function accept(): bool {}
}

class RecursiveCallbackFilterIterator extends CallbackFilterIterator implements RecursiveIterator
{
    public function __construct(RecursiveIterator $iterator, callable $callback) {}

    /**
     * @tentative-return-type
     * @implementation-alias RecursiveFilterIterator::hasChildren
     */
    public function hasChildren(): bool {}

    /** @tentative-return-type */
    public function getChildren(): RecursiveCallbackFilterIterator {}
}

interface RecursiveIterator extends Iterator
{
    /** @tentative-return-type */
    public function hasChildren(): bool;

    /** @tentative-return-type */
    public function getChildren(): ?RecursiveIterator;
}

class RecursiveIteratorIterator implements OuterIterator
{
    public function __construct(Traversable $iterator, int $mode = RecursiveIteratorIterator::LEAVES_ONLY, int $flags = 0) {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function key(): mixed {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function getDepth(): int {}

    /** @tentative-return-type */
    public function getSubIterator(?int $level = null): ?RecursiveIterator {}

    /** @tentative-return-type */
    public function getInnerIterator(): RecursiveIterator {}

    /** @tentative-return-type */
    public function beginIteration(): void {}

    /** @tentative-return-type */
    public function endIteration(): void {}

    /** @tentative-return-type */
    public function callHasChildren(): bool {}

    /** @tentative-return-type */
    public function callGetChildren(): ?RecursiveIterator {}

    /** @tentative-return-type */
    public function beginChildren(): void {}

    /** @tentative-return-type */
    public function endChildren(): void {}

    /** @tentative-return-type */
    public function nextElement(): void {}

    /** @tentative-return-type */
    public function setMaxDepth(int $maxDepth = -1): void {}

    /** @tentative-return-type */
    public function getMaxDepth(): int|false {}
}

interface OuterIterator extends Iterator
{
    /** @tentative-return-type */
    public function getInnerIterator(): ?Iterator;
}

class IteratorIterator implements OuterIterator
{
    public function __construct(Traversable $iterator, ?string $class = null) {}

    /** @tentative-return-type */
    public function getInnerIterator(): ?Iterator {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function key(): mixed {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function next(): void {}
}

abstract class FilterIterator extends IteratorIterator
{
    /** @tentative-return-type */
    abstract public function accept(): bool;

    public function __construct(Iterator $iterator) {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function next(): void {}
}

abstract class RecursiveFilterIterator extends FilterIterator implements RecursiveIterator
{
    public function __construct(RecursiveIterator $iterator) {}

    /** @tentative-return-type */
    public function hasChildren(): bool {}

    /** @tentative-return-type */
    public function getChildren(): ?RecursiveFilterIterator {}
}

class ParentIterator extends RecursiveFilterIterator
{
    public function __construct(RecursiveIterator $iterator) {}

    /**
     * @tentative-return-type
     * @implementation-alias RecursiveFilterIterator::hasChildren
     */
    public function accept(): bool {}
}

interface SeekableIterator extends Iterator
{
    /** @tentative-return-type */
    public function seek(int $offset): void;
}

class LimitIterator extends IteratorIterator
{
    public function __construct(Iterator $iterator, int $offset = 0, int $limit = -1) {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function seek(int $offset): int {}

    /** @tentative-return-type */
    public function getPosition(): int {}
}

class CachingIterator extends IteratorIterator implements ArrayAccess, Countable, Stringable
{
    public function __construct(Iterator $iterator, int $flags = CachingIterator::CALL_TOSTRING) {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function hasNext(): bool {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function getFlags(): int {}

    /** @tentative-return-type */
    public function setFlags(int $flags): void {}

    /**
     * @param string $key
     * @tentative-return-type
     */
    public function offsetGet($key): mixed {}

    /**
     * @param string $key
     * @tentative-return-type
     */
    public function offsetSet($key, mixed $value): void {}

    /**
     * @param string $key
     * @tentative-return-type
     */
    public function offsetUnset($key): void {}

    /**
     * @param string $key
     * @tentative-return-type
     */
    public function offsetExists($key): bool {}

    /** @tentative-return-type */
    public function getCache(): array {}

    /** @tentative-return-type */
    public function count(): int {}
}

class RecursiveCachingIterator extends CachingIterator implements RecursiveIterator
{
    public function __construct(Iterator $iterator, int $flags = RecursiveCachingIterator::CALL_TOSTRING) {}

    /** @tentative-return-type */
    public function hasChildren(): bool {}

    /** @tentative-return-type */
    public function getChildren(): ?RecursiveCachingIterator {}
}

class NoRewindIterator extends IteratorIterator
{
    public function __construct(Iterator $iterator) {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function key(): mixed {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function next(): void {}
}

class AppendIterator extends IteratorIterator
{
    public function __construct() {}

    /** @tentative-return-type */
    public function append(Iterator $iterator): void {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function getIteratorIndex(): ?int {}

    /** @tentative-return-type */
    public function getArrayIterator(): ArrayIterator {}
}

class InfiniteIterator extends IteratorIterator
{
    public function __construct(Iterator $iterator) {}

    /** @tentative-return-type */
    public function next(): void {}
}

class RegexIterator extends FilterIterator
{
    public ?string $replacement = null;

    public function __construct(Iterator $iterator, string $pattern, int $mode = RegexIterator::MATCH, int $flags = 0, int $pregFlags = 0) {}

    /** @tentative-return-type */
    public function accept(): bool {}

    /** @tentative-return-type */
    public function getMode(): int {}

    /** @tentative-return-type */
    public function setMode(int $mode): void {}

    /** @tentative-return-type */
    public function getFlags(): int {}

    /** @tentative-return-type */
    public function setFlags(int $flags): void {}

    /** @tentative-return-type */
    public function getRegex(): string {}

    /** @tentative-return-type */
    public function getPregFlags(): int {}

    /** @tentative-return-type */
    public function setPregFlags(int $pregFlags): void {}
}

class RecursiveRegexIterator extends RegexIterator implements RecursiveIterator
{
    public function __construct(RecursiveIterator $iterator, string $pattern, int $mode = RecursiveRegexIterator::MATCH, int $flags = 0, int $pregFlags = 0) {}

    /** @tentative-return-type */
    public function accept(): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias RecursiveFilterIterator::hasChildren
     */
    public function hasChildren(): bool {}

    /** @tentative-return-type */
    public function getChildren(): RecursiveRegexIterator {}
}

class RecursiveTreeIterator extends RecursiveIteratorIterator
{
    /** @param RecursiveIterator|IteratorAggregate $iterator */
    public function __construct(
        $iterator,
        int $flags = RecursiveTreeIterator::BYPASS_KEY,
        int $cachingIteratorFlags = CachingIterator::CATCH_GET_CHILD,
        int $mode = RecursiveTreeIterator::SELF_FIRST
    ) {}

    /** @tentative-return-type */
    public function key(): mixed {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function getPrefix(): string {}

    /** @tentative-return-type */
    public function setPostfix(string $postfix): void {}

    /** @tentative-return-type */
    public function setPrefixPart(int $part, string $value): void {}

    /** @tentative-return-type */
    public function getEntry(): string {}

    /** @tentative-return-type */
    public function getPostfix(): string {}
}
