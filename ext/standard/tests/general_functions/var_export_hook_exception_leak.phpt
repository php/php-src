--TEST--
var_export() releases the value of a get hook that raises an exception
--FILE--
<?php
class Thrower {
    function __destruct() {
        throw new Exception('destructor');
    }
}
class Hooked {
    public string $prop {
        get {
            $local = new Thrower;
            return str_repeat('y', 100);
        }
    }
}
try {
    var_export(new Hooked);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
destructor
