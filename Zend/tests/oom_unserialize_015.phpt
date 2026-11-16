--TEST--
OOM unserialize() object __unserialize (2)
--INI--
memory_limit=100M
--FILE--
<?php

class C {
    public function __unserialize($data) {
        str_repeat('a', 200<<20);
    }
}

$payload = 'O:1:"C":0:{}';

var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 5
