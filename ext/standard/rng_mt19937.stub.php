<?php

/** @generate-function-entries */

namespace RNG;

class MT19937 implements RNG64Interface
{
    public function __construct(int $seed) {}
    public function next(): int {}
    /** @throws ValueError */
    public function next64(): int {}
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}
