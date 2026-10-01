--TEST--
DCE must not remove assignments to properties of an object holding a reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php

class Holder {
    public $untyped = 0;
    public int $typed = 0;
}

class Target {
    public $untyped = 0;
    public int $typed = 0;
}

function untyped() {
    $h = new Holder;
    $t = new Target;
    $h->untyped = &$t->untyped;
    $h->untyped = 5;
    var_dump($t->untyped);
}

function typed() {
    $h = new Holder;
    $t = new Target;
    $h->typed = &$t->typed;
    $h->typed = 5;
    var_dump($t->typed);
}

untyped();
typed();

?>
--EXPECT--
int(5)
int(5)
