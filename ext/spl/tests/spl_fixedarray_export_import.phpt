--TEST--
SplFixedArray: __set_state export and import
--FILE--
<?php

function compare_export($var, string $case) {
    $exported = var_export($var, true);

    $imported = eval("return {$exported};");
    $reexported = var_export($imported, true);

    if ($exported == $reexported) {
        echo "Pass {$case}.\n";
    } else {
        echo "Fail {$case}.\nExpected:\n{$exported}\nActual:\n{$reexported}\n";
    }
}

// simple
$fixed = SplFixedArray::__set_state([1, 2, 3]);
compare_export($fixed, 'simple');

// dynamic properties
$fixed->undefined = 'undef';
compare_export($fixed, 'dynamic properties');

// inheritance
class Vec
    extends SplFixedArray
{
    public $type;
}
$vec = new Vec(3);
$vec[0] = 1;
$vec[1] = 2;
$vec[2] = 3;
$vec->type = 'int';

compare_export($vec, 'inheritance');

// dynamic properties and inheritance
$vec->undefined = 'undef';
compare_export($vec, 'dynamic properties and inheritance');

--EXPECT--
Pass simple.
Pass dynamic properties.
Pass inheritance.
Pass dynamic properties and inheritance.
