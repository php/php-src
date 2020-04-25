<?php

/** @generate-function-entries */

class EmptyIterator implements Iterator
{
    /** @return void */
    public function current() {}

    /** @return void */
    public function next() {}

    /** @return void */
    public function key() {}

    /** @return bool */
    public function valid() {}

    /** @return void */
    public function rewind() {}
}

class CallbackFilterIterator extends FilterIterator
{
    public function __construct(Iterator $iterator, callable $callback) {}

    /** @return bool */
    public function accept() {}
}

class RecursiveCallbackFilterIterator extends CallbackFilterIterator implements RecursiveIterator
{
    public function __construct(RecursiveIterator $iterator, callable $callback) {}

    /**
     * @return bool
     * @alias RecursiveFilterIterator::hasChildren
     */
    public function hasChildren() {}

    /** @return RecursiveCallbackFilterIterator */
    public function getChildren() {}
}

interface RecursiveIterator extends Iterator
{
    /** @return bool */
    public function hasChildren();

    /** @return RecursiveIterator */
    public function getChildren();
}

class RecursiveIteratorIterator implements OuterIterator
{
    /** @param Traversable $iterator */
    public function __construct(Traversable $iterator, int $mode = self::LEAVES_ONLY, int $flags = 0) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return mixed */
    public function key() {}

    /** @return mixed */
    public function current() {}

    /** @return void */
    public function next() {}

    /** @return int */
    public function getDepth() {}

    /** @return RecursiveIterator|null */
    public function getSubIterator(int $level = UNKNOWN) {}

    /** @return RecursiveIterator */
    public function getInnerIterator() {}

    /** @return void */
    public function beginIteration() {}

    /** @return void */
    public function endIteration() {}

    /** @return bool|null */
    public function callHasChildren() {}

    /** @return RecursiveIterator|null */
    public function callGetChildren() {}

    /** @return void */
    public function beginChildren() {}

    /** @return void */
    public function endChildren() {}

    /** @return void */
    public function nextElement() {}

    /** @return void */
    public function setMaxDepth(int $max_depth = -1) {}

    /** @return int|false */
    public function getMaxDepth() {}
}

interface OuterIterator extends Iterator
{
    /** @return Iterator */
    public function getInnerIterator();
}

class IteratorIterator implements OuterIterator
{
    /** @param Traversable $iterator */
    public function __construct($iterator, string $class_name = UNKNOWN) {}

    /** @return Iterator|null */
    public function getInnerIterator() {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return mixed */
    public function key() {}

    /** @return mixed */
    public function current() {}

    /** @return void */
    public function next() {}
}

abstract class FilterIterator extends IteratorIterator
{
    /** @return bool */
    abstract public function accept();

    public function __construct(Iterator $iterator) {}

    /** @return void */
    public function rewind() {}

    /** @return void */
    public function next() {}
}

abstract class RecursiveFilterIterator extends FilterIterator implements RecursiveIterator
{
    public function __construct(RecursiveIterator $iterator) {}

    /** @return bool */
    public function hasChildren() {}

    /** @return RecursiveFilterIterator|null */
    public function getChildren() {}
}

class ParentIterator extends RecursiveFilterIterator
{
    public function __construct(RecursiveIterator $iterator) {}

    /**
     * @return bool
     * @alias RecursiveFilterIterator::hasChildren
     */
    public function accept() {}
}

interface SeekableIterator extends Iterator
{
    /** @return void */
    public function seek(int $position);
}

class LimitIterator extends IteratorIterator
{
    public function __construct(Iterator $iterator, int $offset = 0, int $count = -1) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return void */
    public function next() {}

    /** @return int */
    public function seek(int $position) {}

    /** @return int */
    public function getPosition() {}
}

class CachingIterator extends IteratorIterator implements ArrayAccess, Countable
{
    public function __construct(Iterator $iterator, int $flags = self::CALL_TOSTRING) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return void */
    public function next() {}

    /** @return bool */
    public function hasNext() {}

    public function __toString(): string {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /**
     * @param string $index
     * @return mixed
     */
    public function offsetGet($index) {}

    /**
     * @param string $index
     * @param mixed $value
     * @return void
     */
    public function offsetSet($index, $value) {}

    /**
     * @param string $index
     * @return void
     */
    public function offsetUnset($index) {}

    /**
     * @param string $index
     * @return bool
     */
    public function offsetExists($index) {}

    /** @return array */
    public function getCache() {}

    /** @return int */
    public function count() {}
}

class RecursiveCachingIterator extends CachingIterator implements RecursiveIterator
{
    public function __construct(Iterator $iterator, int $flags = self::CALL_TOSTRING) {}

    /** @return bool */
    public function hasChildren() {}

    /** @return RecursiveCachingIterator|null */
    public function getChildren() {}
}

class NoRewindIterator extends IteratorIterator
{
    public function __construct(Iterator $iterator) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return mixed */
    public function key() {}

    /** @return mixed */
    public function current() {}

    /** @return void */
    public function next() {}
}

class AppendIterator extends IteratorIterator
{
    public function __construct() {}

    /** @return void */
    public function append(Iterator $iterator) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return mixed */
    public function current() {}

    /** @return void */
    public function next() {}

    /** @return int */
    public function getIteratorIndex() {}

    /** @return ArrayIterator */
    public function getArrayIterator() {}
}

class InfiniteIterator extends IteratorIterator
{
    public function __construct(Iterator $iterator) {}

    /** @return void */
    public function next() {}
}

class RegexIterator extends FilterIterator
{
    public function __construct(Iterator $iterator, string $regex, int $mode = self::MATCH, int $flags = 0, int $preg_flags = 0) {}

    /** @return bool */
    public function accept() {}

    /** @return int */
    public function getMode() {}

    /** @return void */
    public function setMode(int $mode) {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /** @return string */
    public function getRegex() {}

    /** @return int */
    public function getPregFlags() {}

    /** @return void */
    public function setPregFlags(int $preg_flags) {}
}

class RecursiveRegexIterator extends RegexIterator implements RecursiveIterator
{
    public function __construct(RecursiveIterator $iterator, string $regex, int $mode = self::MATCH, int $flags = 0, int $preg_flags = 0) {}

    /** @return bool */
    public function accept() {}

    /**
     * @return bool
     * @alias RecursiveFilterIterator::hasChildren
     */
    public function hasChildren() {}

    /** @return RecursiveRegexIterator */
    public function getChildren() {}
}

class RecursiveTreeIterator extends RecursiveIteratorIterator
{
    /** @param RecursiveIterator|IteratorAggregate $iterator */
    public function __construct(
        $iterator, int $flags = self::BYPASS_KEY, int $caching_it_flags = CachingIterator::CATCH_GET_CHILD,
        int $mode = self::SELF_FIRST
    ) {}

    /** @return mixed */
    public function key() {}

    /** @return mixed */
    public function current() {}

    /** @return string */
    public function getPrefix() {}

    /** @return void */
    public function setPostfix(string $postfix) {}

    /** @return void */
    public function setPrefixPart(int $part, string $value) {}

    /** @return string */
    public function getEntry() {}

    /** @return string */
    public function getPostfix() {}
}
