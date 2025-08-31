--TEST--
Generators can't be serialized or unserialized
--FILE--
<?php

function gen() { yield; }

$gen = gen();

try {
    serialize($gen);
} catch (Exception $e) {
    echo $e, "\n\n";
}

try {
    var_dump(unserialize('O:9:"Generator":0:{}'));
} catch (Exception $e) {
    echo $e, "\n\n";
}

try {
    var_dump(unserialize('C:9:"Generator":0:{}'));
} catch (Exception $e) {
    echo $e;
}

?>
--EXPECTF--
Exception: Serialization of 'Generator' is not allowed in %s:%d
Stack trace:
#0 %s(%d): serialize(Object(Generator))
#1 {main}

Exception: Unserialization of 'Generator' is not allowed in %s:%d
Stack trace:
#0 %s(%d): unserialize('O:9:"Generator"...')
#1 {main}

Exception: Unserialization of 'Generator' is not allowed in %s:%d
Stack trace:
#0 %s(%d): unserialize('C:9:"Generator"...')
#1 {main}
