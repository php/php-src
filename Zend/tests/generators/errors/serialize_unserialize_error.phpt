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
exception 'Exception' with message 'Serialization of 'Generator' is not allowed' in %s:%d
Stack trace:
#0 %s(%d): serialize(Object(Generator))
#1 {main}


Warning: Erroneous data format for unserializing 'Generator' in %sserialize_unserialize_error.php on line %d

Notice: unserialize(): Error at offset 19 of 20 bytes in %sserialize_unserialize_error.php on line %s
bool(false)
exception 'Exception' with message 'Unserialization of 'Generator' is not allowed' in %s:%d
Stack trace:
#0 %s(%d): unserialize('C:9:"Generator"...')
#1 {main}
