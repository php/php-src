--TEST--
openssl_pkey_export_to_file object to string conversion
--EXTENSIONS--
openssl
--FILE--
<?php

class Test {
    public function __toString(): string {
        return "file://" . __DIR__ . "/private_rsa_1024.key";
    }
}

$path = new Test;
$key = [$path, ""];
@openssl_pkey_export_to_file($key, str_repeat("a", 10000), passphrase: "");
var_dump($key);

?>
--EXPECT--
array(2) {
  [0]=>
  object(Test)#1 (0) {
  }
  [1]=>
  string(0) ""
}
