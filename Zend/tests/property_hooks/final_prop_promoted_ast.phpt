--TEST--
Confirm that the AST indicates final promoted properties
--FILE--
<?php
try {
    assert(false && new class {
        public function __construct(public final $prop) {}
    });
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
AssertionError: assert(false && new class {
    public function __construct(public final $prop) {
    }

})
