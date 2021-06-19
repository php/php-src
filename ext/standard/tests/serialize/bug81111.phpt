--TEST--
Bug #81111 ()
--FILE--
<?php

function check_serialize_throws($obj) {
    try {
        var_dump(serialize($obj));
    } catch (Throwable $e) {
        echo "Caught: " . get_class($e) . "\n";
        echo "Message: " . $e->getMessage() . "\n";
    }
}

function make_alias($obj) {
    class_alias(get_class($obj), 'SomeAlias');
    return new SomeAlias();
}

echo "Case 1: anonymous class\n";
check_serialize_throws(new class () {});

echo "\n";
echo "Case 2: anonymous class with __serialize\n";
check_serialize_throws(new class () {
    public function __serialize() { return []; }
    public function __unserialize($value) { }
});

echo "\n";
echo "Case 3: anonymous class with Serializable\n";
check_serialize_throws(new class () implements Serializable {
    public function serialize() { return ''; }
    public function unserialize(string $ser) { return new self(); }
});

echo "\n";
echo "Case 4: aliased anonymous class with __serialize\n";
$alias = make_alias(new class() {
    public function __serialize() { return []; }
});
check_serialize_throws($alias);

?>
--EXPECTF--
Case 1: anonymous class
Caught: Exception
Message: Serialization of 'class@anonymous' is not allowed

Case 2: anonymous class with __serialize
Caught: Exception
Message: Serialization of 'class@anonymous' is not allowed

Case 3: anonymous class with Serializable

Deprecated: The Serializable interface is deprecated. %s
Caught: Exception
Message: Serialization of 'Serializable@anonymous' is not allowed

Case 4: aliased anonymous class with __serialize
Caught: Exception
Message: Serialization of 'class@anonymous' is not allowed
