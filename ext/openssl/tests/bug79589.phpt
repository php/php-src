--TEST--
Bug #65538: TLS unexpected EOF failure
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
if (!defined("OPENSSL_KEYTYPE_EC")) die("skip EC disabled");
?>
--FILE--
<?php

$release = file_get_contents(
    'https://chromedriver.storage.googleapis.com/LATEST_RELEASE',
    false,
    stream_context_create(['ssl' => ['verify_peer'=> false]])
);
echo gettype($release);

?>
--EXPECT--
string
