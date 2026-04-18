--TEST--
GH-21798: phar rejects oversized OpenSSL signature length field
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (!in_array("OpenSSL", Phar::getSupportedSignatures())) die("skip openssl support required");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
// The openssl phar signature layout (end of file):
//   [sig bytes: signature_len bytes]
//   [signature_len: 4 bytes LE]   <- offset -12 from EOF
//   [sig_flags: 4 bytes]          <- offset -8
//   ["GBMB": 4 bytes magic]       <- offset -4
$src = __DIR__ . '/files/openssl.phar';
$tmp = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
copy($src, $tmp);

$fp = fopen($tmp, 'r+b');
fseek($fp, -12, SEEK_END);
fwrite($fp, pack('V', 0x00200001)); // 2 MiB + 1 — exceeds 1 MiB cap
fclose($fp);

try {
    $phar = new Phar($tmp);
} catch (UnexpectedValueException $e) {
    echo $e->getMessage() . "\n";
}
echo "done\n";
?>
--CLEAN--
<?php @unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
--EXPECTF--
phar "%s" openssl signature length is too large
done
