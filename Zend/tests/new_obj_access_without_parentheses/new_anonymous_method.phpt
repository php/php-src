--TEST--
Immediate method call on new anonymous class object
--FILE--
<?php

new class {
    public function test()
    {
        echo 'called';
    }
}->test();

?>
--EXPECT--
called
