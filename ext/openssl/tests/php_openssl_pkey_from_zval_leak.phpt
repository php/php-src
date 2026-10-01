--TEST--
php_openssl_pkey_from_zval memory leak
--EXTENSIONS--
openssl
--FILE--
<?php

class StrFail {
    public function __toString(): string {
        throw new Error('create a leak');
    }
}

$key = ["", new StrFail];
try {
    openssl_pkey_export_to_file($key, "doesnotmatter");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: create a leak
