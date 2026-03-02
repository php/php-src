<?php

#[Attribute]
class Attr {
    public function __construct(public $value) {}
}

class Foo {
    public function __construct(public $value) {}
}

#[Attr(new Foo(TestEnum::CASES))]
function test() {}
