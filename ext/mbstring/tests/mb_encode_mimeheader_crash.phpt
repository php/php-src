--TEST--
Test mb_encode_mimeheader() function : text encoding with no MIME name
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_encode_mimeheader') or die("skip mb_encode_mimeheader() is not available in this build");
?>
--FILE--
<?php
try {
    var_dump(mb_encode_mimeheader("abc", "UTF7-IMAP", "Q"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
mb_encode_mimeheader(): Argument #2 ($charset) "UTF7-IMAP" cannot be used for MIME header encoding
Done
