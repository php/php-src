--TEST--
GH-15968 BCMath\Number operators may typecast operand
--EXTENSIONS--
bcmath
--FILE--
<?php
class MyString {
    function __toString() {
        return "2";
    }
}

$a = new BCMath\Number("1");
$b = new MyString();
try {
    var_dump($a + $b);
} catch (Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Unsupported operand types: BcMath\Number + MyString
