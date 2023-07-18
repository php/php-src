--TEST--
Bug #78154: SEND_VAR_NO_REF does not always send reference
--FILE--
<?php

namespace {
    try {
        var_dump(similar_text('a', 'a', $c=0x44444444));
        var_dump($c);
    } catch (Throwable $e) {
        echo "Exception: " . $e->getMessage() . "\n";
    }
}
namespace Foo {
    try {
        var_dump(similar_text('a', 'a', $d=0x44444444));
        var_dump($d);
    } catch (\Throwable $e) {
        echo "Exception: " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
Exception: similar_text(): Argument #3 ($percent) could not be passed by reference
Exception: similar_text(): Argument #3 ($percent) could not be passed by reference
