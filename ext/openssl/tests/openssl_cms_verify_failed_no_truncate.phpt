--TEST--
openssl_cms_verify does not truncate output files on verification failure
--EXTENSIONS--
openssl
--FILE--
<?php
$dir = __DIR__ . DIRECTORY_SEPARATOR;
$plain   = $dir . 'cms_verify_nt_plain.tmp';
$signed  = $dir . 'cms_verify_nt_signed.tmp';
$aCrt    = $dir . 'cms_verify_nt_a.crt.tmp';
$aKey    = $dir . 'cms_verify_nt_a.key.tmp';
$bCrt    = $dir . 'cms_verify_nt_b.crt.tmp';
$content = $dir . 'cms_verify_nt_content.tmp';
$p7bout  = $dir . 'cms_verify_nt_p7b.tmp';
$signers = $dir . 'cms_verify_nt_signers.tmp';

function mkpair(string $cn, string $certFile, string $keyFile): void {
    $pk = openssl_pkey_new(['private_key_bits' => 2048, 'private_key_type' => OPENSSL_KEYTYPE_RSA]);
    $csr = openssl_csr_new(['commonName' => $cn], $pk);
    $crt = openssl_csr_sign($csr, null, $pk, 1);
    openssl_x509_export($crt, $crtPem);
    openssl_pkey_export($pk, $pkPem);
    file_put_contents($certFile, $crtPem);
    file_put_contents($keyFile, $pkPem);
}

mkpair('signer-A',    $aCrt, $aKey);
mkpair('untrusted-B', $bCrt, $dir . 'cms_verify_nt_b.key.tmp');

file_put_contents($plain, "hello\n");
if (!openssl_cms_sign($plain, $signed, "file://$aCrt", "file://$aKey", [])) {
    echo "sign failed\n";
    exit(1);
}

$sentinel = "DO-NOT-OVERWRITE\n";
file_put_contents($content, $sentinel);
file_put_contents($p7bout, $sentinel);
file_put_contents($signers, $sentinel);

// Verify against a CA that does NOT include the signer. Chain validation fails.
$r = @openssl_cms_verify($signed, 0, $signers, [$bCrt], null, $content, $p7bout);

echo "verify result: " . var_export($r, true) . "\n";
echo "content sentinel intact?  " . (file_get_contents($content)  === $sentinel ? "YES" : "NO") . "\n";
echo "p7bout sentinel intact?   " . (file_get_contents($p7bout)   === $sentinel ? "YES" : "NO") . "\n";
echo "signers sentinel intact?  " . (file_get_contents($signers)  === $sentinel ? "YES" : "NO") . "\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . DIRECTORY_SEPARATOR;
foreach ([
    'cms_verify_nt_plain.tmp',
    'cms_verify_nt_signed.tmp',
    'cms_verify_nt_a.crt.tmp',
    'cms_verify_nt_a.key.tmp',
    'cms_verify_nt_b.crt.tmp',
    'cms_verify_nt_b.key.tmp',
    'cms_verify_nt_content.tmp',
    'cms_verify_nt_p7b.tmp',
    'cms_verify_nt_signers.tmp',
] as $f) {
    @unlink($dir . $f);
}
?>
--EXPECT--
verify result: false
content sentinel intact?  YES
p7bout sentinel intact?   YES
signers sentinel intact?  YES
