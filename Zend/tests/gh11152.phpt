--TEST--
GH-11152: Allow aliasing namespaces ending in reserved class names
--FILE--
<?php

namespace N\String;

interface I {}

class C implements I {}

use N\String as StringAlias;

function test(StringAlias\I $o) {
    var_dump($o::class);
}

test(new C());

?>
--EXPECT--
string(10) "N\String\C"
