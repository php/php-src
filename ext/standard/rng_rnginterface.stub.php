<?php

/** @generate-function-entries */

namespace RNG;

interface RNGInterface
{
    public function next(): int;
    public function next64(): int;
}
