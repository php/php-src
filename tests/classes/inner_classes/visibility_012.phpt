--TEST--
instantiate nested private classes
--FILE--
<?php
class Outer {
    private class Middle {
        public class Inner {}
    }

    private Middle\Inner $inner;

    public function __construct() {
        $this->inner = new Middle\Inner();
    }
}

$x = new Outer();
echo "success\n";
?>
--EXPECT--
success
