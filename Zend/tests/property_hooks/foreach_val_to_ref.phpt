--TEST--
foreach by-ref on object with by-val hooked property
--FILE--
<?php

class ByVal {
    public $byVal = 'byValue' {
        get => $field;
        set => $field = $value;
    }
}

function test($object) {
    foreach ($object as $prop => &$value) {
        $value = strtoupper($value);
    }
    var_dump($object);
}

try {
    test(new ByVal);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot create reference to property ByVal::$byVal
