--TEST--
`never` parameter types - AST output from assertions
--FILE--
<?php

try {
    assert(false && new class {
        public function invalid(never $v) {}
    });
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
assert(false && new class {
    public function invalid(never $v) {
    }

})
