--TEST--
Bug #77092: array_diff_key() - segmentation fault
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    $anyArrayOne = ['foo' => 'bar', 'bar' => 'baz'];
    $anyArrayTwo = ['foo' => null];

    print_r(array_diff_key($anyArrayOne, $anyArrayTwo));
}
test();
?>
--EXPECT--
Array
(
    [bar] => baz
)
