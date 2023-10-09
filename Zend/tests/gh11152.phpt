--TEST--
GH-11152: Allow aliasing namespaces containing reserved class names
--FILE--
<?php

namespace string;

use string as StringAlias;

class C {}

function test(StringAlias\C $o) {
    var_dump($o::class);
}

test(new C());

?>
--EXPECT--
string(8) "string\C"
