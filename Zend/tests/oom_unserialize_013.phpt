--TEST--
OOM unserialize() object __wakeup
--INI--
memory_limit=100M
--FILE--
<?php

class C {
    public function __wakeup() {
        str_repeat('a', 200*1024*1024);
    }
}

class D {
    public function __wakeup() {
        echo "Not executed";
        // Would hit hard limit
        zend_test_emalloc(200*1024*1024);
    }
}

$payload = 'a:2:{i:0;O:1:"C":0:{}i:1;O:1:"D":0:{}}';

var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 5
