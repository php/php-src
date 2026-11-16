--TEST--
AST printing of fully qualified and namespace-relative names in name lists
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && function () {
        class C implements \A, namespace\B, C {
            use \T1, namespace\T2, T3 {
                \T1::foo insteadof namespace\T2, T3;
                namespace\T2::foo as bar;
            }
        }
        try {
        } catch (\E1 | namespace\E2 | E3 $e) {
        }
    });
} catch (AssertionError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && function () {
    class C implements \A, namespace\B, C {
        use \T1, namespace\T2, T3 {
            \T1::foo insteadof namespace\T2, T3;
            namespace\T2::foo as bar;
        }
    }

    try {
    } catch (\E1|namespace\E2|E3 $e) {
    }
})
