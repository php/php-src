--TEST--
Bug #71922: Crash on assert(new class{});
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

try {
    assert(0 && new class {
    } && new class(42) extends stdclass {
    });
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(0 && new class {
} && new class(42) extends stdclass {
})
