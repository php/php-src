--TEST--
Bug GH-8750 (Can not create VT_ERROR variant type) - cast and conversion
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$error = new variant(DISP_E_PARAMNOTFOUND, VT_ERROR);

// explicit variant_cast() to int is supported if in range
echo variant_cast($error, VT_I4), PHP_EOL;

// however, explicit (int) casts are not supported
echo (int) $error, PHP_EOL;

// nor are implicit conversions
try {
    echo 1 + $error, PHP_EOL;
} catch (TypeError $err) {
    echo $err->getMessage(), PHP_EOL;
}

// we can retrieve the type
echo variant_get_type($error), PHP_EOL;

// and change it via variant_set_type()
variant_set_type($error, VT_I4);
echo variant_get_type($error), PHP_EOL;
?>
--EXPECTF--
-2147352572

Warning: Object of class variant could not be converted to int in %s on line %d
1
Unsupported operand types: int + variant
10
3
