--TEST--
Cannot define a return type on a Generator

--FILE--
<?php

function filter(Traversable $in, callable $filter): array {
    foreach ($in as $key => $value) {
        if ($filter($key, $value)) {
            yield $key => $value;
        }
    }
}

--EXPECTF--
Fatal error: The "yield" expression can not be used inside a function with a return type hint in %s on line 6
