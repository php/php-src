--TEST--
GHSA-68fq-fh7h-f8gr (OpenSSL signature bypass by downgrading the algorithm to a keyless one)
--EXTENSIONS--
phar
openssl
--SKIPIF--
<?php
if (!in_array("OpenSSL", Phar::getSupportedSignatures())) die("skip openssl support required");
?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php
$base = __DIR__ . '/' . basename(__FILE__, '.php');
$private = openssl_pkey_get_private(file_get_contents(__DIR__ . '/files/private.pem'));
$pkey = '';
openssl_pkey_export($private, $pkey, null, ['config' => __DIR__ . '/files/openssl.cnf']);
$pubkey = openssl_pkey_get_details($private)['key'];

function build(string $file, string $pkey): string
{
    $phar = new Phar($file);
    $phar['main.php'] = '<?php echo "GENUINE\n";';
    $phar->setStub('<?php __HALT_COMPILER();');
    $phar->setSignatureAlgorithm(Phar::OPENSSL, $pkey);

    return file_get_contents($file);
}

// a phar built in this process comes back from the cache, so each case is read under its own name
function open(string $file, string $archive, ?string $pubkey): void
{
    file_put_contents($file, $archive);
    if ($pubkey !== null) {
        file_put_contents($file . '.pubkey', $pubkey);
    }

    try {
        echo (new Phar($file))->getSignature()['hash_type'], "\n";
    } catch (UnexpectedValueException $e) {
        echo $e->getMessage(), "\n";
    }
}

function downgrade_phar(string $archive): string
{
    // the last 12 bytes are [length][algorithm]["GBMB"], the signature sits just before them
    $siglen = unpack('V', substr($archive, -12, 4))[1];
    $message = substr($archive, 0, strlen($archive) - 12 - $siglen);

    return $message . hash('sha256', $message, true) . pack('V', 0x0003) . 'GBMB'; // 0x0003 is PHAR_SIG_SHA256
}

function downgrade_tar(string $archive): string
{
    for ($offset = 0; $offset + 512 <= strlen($archive); ) {
        $size = (int) octdec(trim(substr($archive, $offset + 124, 12), " \0")); // octal size field of a 512 byte header

        if (rtrim(substr($archive, $offset, 100), "\0") === '.phar/signature.bin') {
            // the signed message stops at this header, so rewriting the entry content leaves it alone
            $blob = pack('VV', 0x0003, 32) . hash('sha256', substr($archive, 0, $offset), true); // [algorithm][digest length][digest]

            return substr_replace($archive, str_pad($blob, $size, "\0"), $offset + 512, $size);
        }

        $offset += 512 + (($size + 511) & ~511);
    }

    throw new RuntimeException('no signature entry');
}

function downgrade_zip(string $archive): string
{
    $eocd = strrpos($archive, "PK\x05\x06");
    $cdir = unpack('V', substr($archive, $eocd + 16, 4))[1]; // the eocd holds the directory offset at +16 and the archive comment length at +20
    $comment = substr($archive, $eocd + 22, unpack('v', substr($archive, $eocd + 20, 2))[1]);

    for ($record = $cdir; substr($archive, $record, 4) === "PK\x01\x02"; ) {
        $namelen = unpack('v', substr($archive, $record + 28, 2))[1];

        if (substr($archive, $record + 46, $namelen) === '.phar/signature.bin') {
            $size = unpack('V', substr($archive, $record + 24, 4))[1];
            $local = unpack('V', substr($archive, $record + 42, 4))[1];
            $content = $local + 30 + unpack('v', substr($archive, $local + 26, 2))[1] + unpack('v', substr($archive, $local + 28, 2))[1];
            // the signed message is everything before the local entry plus the directory up to this record
            $message = substr($archive, 0, $local) . substr($archive, $cdir, $record - $cdir) . $comment;
            $blob = pack('VV', 0x0003, 32) . hash('sha256', $message, true);

            return substr_replace($archive, str_pad($blob, $size, "\0"), $content, $size);
        }

        $record += 46 + $namelen + unpack('v', substr($archive, $record + 30, 2))[1] + unpack('v', substr($archive, $record + 32, 2))[1];
    }

    throw new RuntimeException('no signature entry');
}

foreach (['phar' => 'downgrade_phar', 'phar.tar' => 'downgrade_tar', 'phar.zip' => 'downgrade_zip'] as $suffix => $downgrade) {
    $archive = build($base . '.source.' . $suffix, $pkey);
    open($base . '.signed.' . $suffix, $archive, $pubkey);
    open($base . '.tampered.' . $suffix, $downgrade($archive), $pubkey);
    open($base . '.nokey.' . $suffix, $downgrade($archive), null);
}
?>
--CLEAN--
<?php
$base = __DIR__ . '/' . basename(__FILE__, '.clean.php');
foreach (glob($base . '.*.phar*') as $file) {
    unlink($file);
}
?>
--EXPECTF--
OpenSSL
phar "%stampered.phar" SHA256 signature could not be verified: openssl public key found, but the signature does not use openssl
SHA-256
OpenSSL
phar error: tar-based phar "%stampered.phar.tar" signature cannot be verified: openssl public key found, but the signature does not use openssl
SHA-256
OpenSSL
phar error: signature cannot be verified: openssl public key found, but the signature does not use openssl in zip-based phar "%stampered.phar.zip"
SHA-256
