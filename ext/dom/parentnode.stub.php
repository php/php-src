<?php

interface DOMParentNode
{
    /** @var ...DOMNode|string $nodes */
    public function append(...$nodes): void;

    /** @var ...DOMNode|string $nodes */
    public function prepend(...$nodes): void;
}
