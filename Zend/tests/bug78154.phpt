--TEST--
Bug #78154: SEND_VAR_NO_REF does not always send reference
--FILE--
<?php

namespace {
    var_dump(similar_text('a', 'a', $c=0x44444444));
    var_dump($c);
}
namespace Foo {
    var_dump(similar_text('a', 'a', $d=0x44444444));
    var_dump($d);
}

?>
--EXPECTF--
Notice: Only variables should be passed by reference in %s on line %d
int(1)
int(1145324612)

Notice: Only variables should be passed by reference in %s on line %d
int(1)
int(1145324612)
