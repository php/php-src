--TEST--
Memory leak when array initialization in openssl_pkcs12_read() fails
--EXTENSIONS--
openssl
--FILE--
<?php
$pfx = __DIR__ . DIRECTORY_SEPARATOR . "bug74022.pfx";
$cert_store = file_get_contents($pfx);

class Typed {
    public string $foo = "bar";
}

$typed = new Typed;

try {
    openssl_pkcs12_read($cert_store, $typed->foo, "csos");
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Cannot assign array to reference held by property Typed::$foo of type string
