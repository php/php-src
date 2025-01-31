--TEST--
Foo(...) in attribute in assert
--FILE--
<?php
assert(function() {
    #[Foo(...)]
    class Test {}
});
?>
--EXPECTF--
Fatal error: Cannot create Closure as attribute argument in %s on line %d
