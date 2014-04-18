--TEST--
Fully qualified classes are allowed in return types

--FILE--
<?php

namespace Collections;

interface Fooable {
   function foo(\Iterator $i): \Iterator;
}

class Foo implements Fooable {
    function foo(\Iterator $i): \Iterator {
        return $i;
    }
}

--EXPECTF--
