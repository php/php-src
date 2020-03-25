--TEST--
Test deflate_add() buffer issue with data that fills deflate buffer while using ZLIB_SYNC_FLUSH on ZLIB_ENCODING_RAW.
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php

/*
 * When using ZLIB_ENCODING_RAW, the deflated buffer should always end in 00 00 ff ff
 * Many streaming deflate users rely on this behaviour.
 * example: websocket permessage-deflate extension
 * (https://tools.ietf.org/html/draft-ietf-hybi-permessage-compression-28#section-7.2.1)
 *
 * Prior to fixing, the output buffer size was not being checked. According to the zlib
 * manual, deflate must be called again with more buffer space.
 */

$deflateContext = deflate_init(ZLIB_ENCODING_RAW);

$deflated = deflate_add(
    $deflateContext,
    hex2bin("255044462d312e320a25c7ec8fa20a362030206f626a0a3c3c2f4c656e6774682037203020522f46696c746572202f466c6174654465636f64653e3e0a737472"),
    ZLIB_SYNC_FLUSH
);

echo bin2hex(substr($deflated, strlen($deflated) - 4)) . "\n";

$deflated = deflate_add(
    $deflateContext,
    hex2bin("65616d0a789c7d53c16ed43010bde7c037f85824766a7bc6767c2ca8a00a016a1b2edcb2dbecaed1266937d98afe3d6327363794439437e3f17b6f5e242821e3"),
    ZLIB_SYNC_FLUSH
);

echo bin2hex(substr($deflated, strlen($deflated) - 4)) . "\n";

$deflated = deflate_add(
    $deflateContext,
    hex2bin("b3be777df5525d3f90384cd58b50a9945fbb5e7c6cb8c89fca8156c688665f2de794504a81f75658a7c1d54a347d7575fb6e17ba617edffcae9c84da3aee6c9e"),
    ZLIB_SYNC_FLUSH
);

echo bin2hex(substr($deflated, strlen($deflated) - 4)) . "\n";
?>
--EXPECT--
0000ffff
0000ffff
0000ffff
