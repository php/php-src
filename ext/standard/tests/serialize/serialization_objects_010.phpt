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
Deprecated: The Serializable interface is deprecated. If you need to retain the Serializable interface for cross-version compatibility, you can suppress this warning by implementing __serialize() and __unserialize() in addition, which will take precedence over Serializable in PHP versions that support them in %s on line %d
C::serialize() must return a string or NULL
Done
