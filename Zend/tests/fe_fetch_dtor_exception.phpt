--TEST--
Dtor may throw exception furing FE_FETCH assignment
--FILE--
<?php

$v = new class {
    function __destruct() {
        throw new Exception("foo");
    }
};

try {
    foreach ([1, 2] as $v) {
        var_dump($v);
    }
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
foo
