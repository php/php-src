<?php

class Foo {
    public $prop;
}

class Bar extends Foo {
    public $prop = self::FOOBAR;
    const FOOBAR = 42;
}
