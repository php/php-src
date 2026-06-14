--TEST--
String cast with reference __toString
--FILE--
<?php

class MyClass {
    private $field = 'my string';
    public function &__toString(): string {
        return $this->field;
    }
}

echo new MyClass;

?>
--EXPECT--
my string
