--TEST--
Hook parameters accept parameter-targeted attributes
--FILE--
<?php

class C {
    public $prop {
        set(#[SensitiveParameter] $value) {
            throw new Exception('Exception from $prop');
        }
    }
}

$c = new C();
try {
    $c->prop = 'secret';
} catch (Exception $e) {
    echo $e;
}

?>
--EXPECTF--
Exception: Exception from $prop in %s:%d
Stack trace:
#0 %s(%d): C->$prop::set(Object(SensitiveParameterValue))
#1 {main}
