--TEST--
Constructor for GMP
--EXTENSIONS--
gmp
--FILE--
<?php
var_dump(new GMP);
var_dump(new GMP(0));
var_dump(new GMP(123));
var_dump(new GMP("0xAA"));
var_dump(new GMP("12", 4));
try {
    var_dump(new GMP("12", 999));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump(new GMP("", 10));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump(new GMP("hello"));
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "123"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "170"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(1) "6"
}
GMP::__construct(): Argument #2 ($base) must be between 2 and 62
GMP::__construct(): Argument #1 ($num) is not an integer string
GMP::__construct(): Argument #1 ($num) is not an integer string
