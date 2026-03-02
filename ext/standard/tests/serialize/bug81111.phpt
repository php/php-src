--TEST--
Bug #81111: Serialization is unexpectedly allowed on anonymous classes with __serialize()
--FILE--
<?php

class MySplFileInfo extends SplFileInfo {
    public function __serialize() { return []; }
    public function __unserialize($value) { return new self('file'); }
}

try {
    serialize(new MySplFileInfo(__FILE__));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

$anon = new class () {
    public function __serialize() { return []; }
    public function __unserialize($value) { }
};

try {
    serialize($anon);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    unserialize("O:13:\"MySplFileInfo\":0:{}");
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    unserialize("C:13:\"MySplFileInfo\":0:{}");
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

$name = $anon::class;
try {
    unserialize("O:" . strlen($name) . ":\"" . $name . "\":0:{}");
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Serialization of 'MySplFileInfo' is not allowed
Serialization of 'class@anonymous' is not allowed
Unserialization of 'MySplFileInfo' is not allowed
Unserialization of 'MySplFileInfo' is not allowed

Warning: unserialize(): Error at offset 0 of %d bytes in %s on line %d
