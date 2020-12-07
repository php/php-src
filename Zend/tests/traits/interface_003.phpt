--TEST--
Testing to implement Serializable interface by traits
--FILE--
<?php

trait foo {
    public function serialize() {
        return 'foobar';
    }
    public function unserialize($x) {
        var_dump($x);
    }
}

class bar implements Serializable {
    use foo;
}

var_dump($o = serialize(new bar));
var_dump(unserialize($o));

?>
--EXPECTF--
Deprecated: The Serializable interface is deprecated. If you need to retain the Serializable interface for cross-version compatibility, you can suppress this warning by implementing __serialize() and __unserialize() in addition, which will take precedence over Serializable in PHP versions that support them in %s on line %d
string(20) "C:3:"bar":6:{foobar}"
string(6) "foobar"
object(bar)#%d (0) {
}
