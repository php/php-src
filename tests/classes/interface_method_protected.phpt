--TEST--
Ensure an interface can have protected methods
--FILE--
<?php
interface I {
    protected function x();
    protected static function y();
}

class Cl implements I {
    protected function x() {
    }

    protected static function y() {
    }
}

echo "Done\n";
?>
--EXPECT--
Done
