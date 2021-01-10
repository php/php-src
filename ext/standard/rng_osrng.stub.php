<?php

/** @generate-function-entries */

namespace RNG;

class OSRNG implements RNG64Interface
{
    public function next(): int {}
    /** @throws ValueError */
    public function next64(): int {}
}
