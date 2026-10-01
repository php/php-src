--TEST--
Bug #27504 (call_user_func_array allows calling of private/protected methods)
--FILE--
<?php
class foo {
    function __construct () {
        $this->bar('1');
    }
    private function bar ( $param ) {
        echo 'Called function foo:bar('.$param.')'."\n";
    }
}

$foo = new foo();

try {
    call_user_func_array( array( $foo , 'bar' ) , array( '2' ) );
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $foo->bar('3');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Called function foo:bar(1)
TypeError: call_user_func_array(): Argument #1 ($callback) must be a valid callback, cannot access private method foo::bar()
Error: Call to private method foo::bar() from global scope
