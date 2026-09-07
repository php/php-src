--TEST--
OSS-Fuzz #471486164: get_property_ptr_ptr() on uninitialized hooked property
--FILE--
<?php

class C {
    public int $a {
        get => $this->a;
        set {
            global $ref;
            $this->a = &$ref;
        }
    }
}

$ref = 1;
$proxy = new C;
$proxy->a = 1;
var_dump($proxy->a);
$ref++;
var_dump($proxy->a);

?>
--EXPECT--
int(1)
int(2)
