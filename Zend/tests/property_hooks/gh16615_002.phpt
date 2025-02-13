--TEST--
GH-16615 002 (Assertion failure in zend_std_read_property)
--FILE--
<?php

class Foo {
    public string $bar {
        set => $value;
    }
    public function __clone() {
        try {
            echo $this->bar;
        } catch (Error $e) {
            printf("%s: %s\n", $e::class, $e->getMessage());
        }
    }
}

// Adds IS_PROP_REINITABLE to prop flags
clone new Foo();

?>
--EXPECT--
Error: Typed property Foo::$bar must not be accessed before initialization
