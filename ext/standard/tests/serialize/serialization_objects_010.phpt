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
--EXPECT--
C::serialize() must return a string or NULL
Done
