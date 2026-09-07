<?php

#[Attribute]
class A {
    function __construct(public mixed $fn) {}
}

#[A(strlen(?))]
class C {
    const CC = strlen(?);

    #[A(strlen(?))]
    function f($arg = strlen(?)) {
        return $arg;
    }
}

const CC = strlen(?);

?>
