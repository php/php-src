--TEST--
Memory leak of X509_STORE in php_openssl_setup_verify() on failure
--EXTENSIONS--
openssl
--FILE--
<?php

class MyStringable{
    public function __toString(): string {
        throw new Error('stop');
    }
}

try {
    openssl_pkcs7_verify("does not matter", 0, "does not matter", [new MyStringable]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
stop
