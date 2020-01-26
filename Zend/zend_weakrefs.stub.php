<?php

final class WeakReference {
    public function __construct();

    public static function create(): WeakReference;

    public function get(): ?object;
}

final class WeakMap implements ArrayAccess, Countable, Traversable {
    //public function __construct();

    /**
     * @param object $object
     * @return mixed
     */
    public function offsetGet($object);

    /**
     * @param object $object
     * @param mixed $value
     */
    public function offsetSet($object, $value): void;

    /** @param object $object */
    public function offsetExists($object): bool;

    /** @param object $object */
    public function offsetUnset($object): void;

    public function count(): int;
}
