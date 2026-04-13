--TEST--
Generic class: new with explicit type arguments
--FILE--
<?php
class Box<T> {
    private $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

$intBox = new Box<int>(42);
echo $intBox->get() . "\n";

$strBox = new Box<string>("hello");
echo $strBox->get() . "\n";
?>
--EXPECT--
42
hello
