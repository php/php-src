--TEST--
Bug #32427 (Interfaces are not allowed 'static' access modifier)
--FILE--
<?php

interface Example {
    public static function sillyError();
}

class ExampleImpl implements Example {
    public static function sillyError() {
        echo "I am a silly error\n";
    }
}

ExampleImpl::sillyError();
?>
--EXPECT--
I am a silly error
