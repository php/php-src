--TEST--
OSS-Fuzz #382922236: Duplicate dynamic properties in hooked object iterator properties table
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $a {
        get => 42;
    }
}

$obj = new C();
$b = &$obj->b;
unset($b);
echo json_encode($obj);

?>
--EXPECT--
{"a":42,"b":null}
