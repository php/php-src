<?php

/** @generate-function-entries */

namespace RNG;

interface RNG64Interface extends RNGInterface
{
    /** @throws ValueError */
    public function next64(): int;
}
