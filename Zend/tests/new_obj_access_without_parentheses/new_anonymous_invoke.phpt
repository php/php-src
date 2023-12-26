--TEST--
Immediate invocation of new anonymous class object
--FILE--
<?php

new class {
    public function __invoke()
    {
        echo 'invoked';
    }
}();

?>
--EXPECT--
invoked
