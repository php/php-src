--TEST--
inner anonymous classes
--FILE--
<?php

$a = new class {
    public class Foo {
        public function bar() {
            echo __METHOD__ . "\n";
        }
    }
};

new $a::Foo()->bar();
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "Foo", expecting variable or "$" in %s on line %d
