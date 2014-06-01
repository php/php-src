--TEST--
Bug #64354 (Unserialize array of objects whose class can't be autoloaded fail)
--FILE--
<?php
spl_autoload_register(
    function($class) {
        throw new Exception("Failed");
    }
);

try {
    var_dump(unserialize('O:1:"A":0:{}'));
} catch (Exception $e) { 
    var_dump($e->getMessage());
}

try {
    var_dump(unserialize('a:2:{i:0;O:1:"A":0:{}i:1;O:1:"A":0:{}}'));
} catch (Exception $e) { 
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(6) "Failed"
string(6) "Failed"
