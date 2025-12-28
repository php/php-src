<?php

#[Attr(static function () { })]
#[Attr(static function (...$args) {
    var_dump($args);
})]
class C {}

?>
