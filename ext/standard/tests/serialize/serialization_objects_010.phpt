--TEST--
Serialize() must return a string or NULL
--FILE--
<?php
Class C implements Serializable {
    public function serialize() {
        return $this;
    }

    public function unserialize($blah) {
    }
}

try {
    var_dump(serialize(new C));
} catch (Exception $e) {
    echo $e->getMessage(). "\n";
}

echo "Done";
?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
C::serialize() must return a string or NULL
Done
