--TEST--
BcMath\Number serialize
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    '0',
    '0.0',
    '2',
    '1234',
    '12.0004',
    '0.1230',
    1,
    12345,
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);
    echo serialize($num) . "\n";
}
?>
--EXPECT--
O:13:"BcMath\Number":1:{s:5:"value";s:1:"0";}
O:13:"BcMath\Number":1:{s:5:"value";s:3:"0.0";}
O:13:"BcMath\Number":1:{s:5:"value";s:1:"2";}
O:13:"BcMath\Number":1:{s:5:"value";s:4:"1234";}
O:13:"BcMath\Number":1:{s:5:"value";s:7:"12.0004";}
O:13:"BcMath\Number":1:{s:5:"value";s:6:"0.1230";}
O:13:"BcMath\Number":1:{s:5:"value";s:1:"1";}
O:13:"BcMath\Number":1:{s:5:"value";s:5:"12345";}
