<?php

/** @generate-function-entries */

namespace RNG;

class XorShift128Plus implements RNGInterface
{
    public function __construct(int $seed) {}
    public function next(): int {}
    public function next64(): int {}
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}
