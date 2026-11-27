--TEST--
get_object_vars() releases the value of a get hook that raises an exception
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
    get_object_vars(new Hooked);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
destructor
