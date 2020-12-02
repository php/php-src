--TEST--
Use of @ before foreach value to iterator upon
--FILE--
<?php

function foo() {
    throw new Exception();
}

foreach (@foo() as $val) {
    var_dump($val);
}

echo "Done\n";
?>
--EXPECTF--
Warning: foreach() argument must be of type array|object, null given in %s on line %d
Done
