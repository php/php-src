--TEST--
AST can be recreated (readonly promoted constructor parameters)
--EXTENSIONS--
zend_test
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly int $publicReadonly,
        readonly int $implicitlyPublicReadonly,
        protected private(set) readonly array $asymmetricReadonly = [],
        #[Attr] private readonly ?string $attributedReadonly = null,
        public int $notReadonly = 1,
    ) {}
}

echo zend_test_compile_to_ast( file_get_contents( __FILE__ ) );

?>
--EXPECT--
class Foo {
    public function __construct(public readonly int $publicReadonly, readonly int $implicitlyPublicReadonly, protected private(set) readonly array $asymmetricReadonly = [], #[Attr] private readonly ?string $attributedReadonly = null, public int $notReadonly = 1) {
    }

}

echo zend_test_compile_to_ast(file_get_contents(__FILE__));
