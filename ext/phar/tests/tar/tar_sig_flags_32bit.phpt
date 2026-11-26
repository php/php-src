--TEST--
Phar: tar-based phar must validate all 32 bits of sig_flags in .phar/signature.bin
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$base = __DIR__ . '/h14_tar_sigflags';
@unlink("$base.phar");
@unlink("$base.phar.tar");

$p = new Phar("$base.phar");
$p = $p->convertToExecutable(Phar::TAR);
$p->setSignatureAlgorithm(Phar::SHA256);
$p['a.txt'] = 'hello';
unset($p);

$raw = file_get_contents("$base.phar.tar");
$pos = strpos($raw, ".phar/signature.bin\0");
$dataOff = $pos + 512;

/* Corrupt each of the 3 upper bytes of little-endian sig_flags separately:
 * byte 1: 03 00 00 00 -> 03 ff 00 00
 * byte 2: 03 00 00 00 -> 03 00 ff 00
 * byte 3: 03 00 00 00 -> 03 00 00 ff
 */
foreach ([1, 2, 3] as $offset) {
    $badFile = "{$base}_bad{$offset}.phar.tar";
    @unlink($badFile);
    $bad = $raw;
    $bad[$dataOff + $offset] = "\xFF";
    file_put_contents($badFile, $bad);

    try {
        new Phar($badFile);
        echo "byte {$offset}: UNEXPECTED_SUCCESS\n";
    } catch (UnexpectedValueException $e) {
        echo "byte {$offset}: " . $e->getMessage() . "\n";
    }
}
?>
--CLEAN--
<?php
$base = __DIR__ . '/h14_tar_sigflags';
@unlink("$base.phar");
@unlink("$base.phar.tar");
foreach ([1, 2, 3] as $offset) {
    @unlink("{$base}_bad{$offset}.phar.tar");
}
?>
--EXPECTF--
byte 1: phar error: tar-based phar "%sh14_tar_sigflags_bad1.phar.tar" signature cannot be verified: broken or unsupported signature
byte 2: phar error: tar-based phar "%sh14_tar_sigflags_bad2.phar.tar" signature cannot be verified: broken or unsupported signature
byte 3: phar error: tar-based phar "%sh14_tar_sigflags_bad3.phar.tar" signature cannot be verified: broken or unsupported signature
