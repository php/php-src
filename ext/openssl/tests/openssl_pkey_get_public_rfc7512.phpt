--TEST--
openssl_pkey_get_public(), openss_pkey_get_private(), openssl_x509_read() with RFC7512 URI
--SKIPIF--
<?php
if (!extension_loaded("openssl"))
    die("skip");
if (!function_exists("proc_open")) die("skip no proc_open");
$PKCS11_MODULE_PATH="/usr/lib/softhsm/libsofthsm2.so";
exec('openssl help', $out, $code);
if ($code > 0) die("skip couldn't locate openssl binary");
exec('softhsm2-util --version', $out, $code);
if ($code > 0) die("skip couldn't locate softhsm2-util binary");
exec('pkcs11-tool --show-info --module ' . $PKCS11_MODULE_PATH, $out, $code);
if ($code > 0) die("skip couldn't locate pkcs11-tool binary");
exec('pkcs11-dump info ' . $PKCS11_MODULE_PATH, $out, $code);
if ($code > 0) die("skip couldn't locate pkcs11-dump binary");
?>
--FILE--
<?php

/* simple exec */
function sexec($cmd, &$stdout=null, &$stderr=null) {
    $proc = proc_open(
        $cmd,
        [
            1 => ['pipe','w'],
            2 => ['pipe','w'],
        ],
        $pipes
    );

    $stdout = stream_get_contents($pipes[1]);
    fclose($pipes[1]);

    $stderr = stream_get_contents($pipes[2]);
    fclose($pipes[2]);

    return proc_close($proc);
}

$PKCS11_MODULE_PATH="/usr/lib/softhsm/libsofthsm2.so";
putenv("PKCS11_MODULE_PATH=".$PKCS11_MODULE_PATH);

$SOFTHSM2_CONF=tempnam(sys_get_temp_dir(), 'softhsm2');
$SOFTHSM2_TOKENDIR=sprintf("%s.dir", $SOFTHSM2_CONF);
mkdir($SOFTHSM2_TOKENDIR);
$PHP11_PIN=123456;
$PHP11_SOPIN=12345678;

file_put_contents(
    $SOFTHSM2_CONF,
    sprintf(
        "directories.tokendir = %s" . PHP_EOL.
        "objectstore.backend = file" . PHP_EOL.
        "log.level = DEBUG" . PHP_EOL.
        "slots.removable = false" . PHP_EOL.
        "slots.mechanisms = ALL",
        $SOFTHSM2_TOKENDIR
    )
);

putenv(sprintf("SOFTHSM2_CONF=%s", $SOFTHSM2_CONF));
sexec("softhsm2-util --show-slots | grep ^Slot | cut -d ' ' -f 2", $out);
$INIT11_SLOT=(int)$out[0];
if ($INIT11_SLOT != 0) {
    echo "Error slot";
    exec("softhsm2-util --show-slots", $out);
    var_dump($out);
    exit(1);
}

sexec(sprintf("softhsm2-util --init-token --free --slot %d --label TestVJToken --pin %s --so-pin %s",
             $INIT11_SLOT, $PHP11_PIN, $PHP11_SOPIN), $out);

/* XXX custom slot is always the first one */
sexec(sprintf("pkcs11-dump slotlist %s 2>/dev/null | grep SoftHSM | head -1 | cut -f 1",
              $PKCS11_MODULE_PATH), $PHP11_SLOT);
if (!is_string($PHP11_SLOT)) {
    echo "Cannot detect the slot" . PHP_EOL;
    exit(1);
}
$PHP11_SLOT=(int)$PHP11_SLOT;

/*
 * Most of these features can be supported natively by PHP, but
 * the purpose is to focus on RFC7512, so let's use the system tools.
 */
$PRIVKEYPEM=$SOFTHSM2_TOKENDIR . '.key.priv.pem';
$PRIVKEYDER=$SOFTHSM2_TOKENDIR . '.key.priv.der';
$PUBKEYDER=$SOFTHSM2_TOKENDIR . '.key.pub.der';
$CERTDER=$SOFTHSM2_TOKENDIR . '.cert.der';
sexec(sprintf("openssl genrsa -out %s 2048", $PRIVKEYPEM), $genkey);
sexec(sprintf("openssl rsa -inform PEM -in %s -outform DER -out %s", $PRIVKEYPEM, $PRIVKEYDER), $pem2der);

/* extract the public key */
sexec(sprintf("openssl rsa -in %s -outform DER -pubout -out %s", $PRIVKEYPEM, $PUBKEYDER), $extract);

/* let's import these keys */
sexec(sprintf("pkcs11-tool --login --pin %s --write-object %s --type privkey --label VJPrivKey --module %s",
              $PHP11_PIN, $PRIVKEYDER, $PKCS11_MODULE_PATH), $importpriv);

sexec(sprintf("pkcs11-tool --login --pin %s --write-object %s --type pubkey  --label VJPubKey  --module %s",
              $PHP11_PIN, $PUBKEYDER, $PKCS11_MODULE_PATH), $importpub);

/* let's build the x509 */
sexec(sprintf("openssl req -new -x509 -subj '/CN=MyCertVJ' ".
                 "-engine pkcs11 -keyform engine -key 'pkcs11:object=VJPrivKey;type=private;pin-value=%s' ".
                 "-outform DER -out %s", $PHP11_PIN, $CERTDER), $req);
sexec(sprintf("pkcs11-tool --login --pin %s --write-object %s --type cert    --label VJCert    --module %s",
              $PHP11_PIN, $CERTDER, $PKCS11_MODULE_PATH), $importcert);

/* let's start the tests */

$key = openssl_pkey_get_private(sprintf("pkcs11:object=VJPrivKey;type=private;pin-value=%s", $PHP11_PIN));
if (!($key instanceof OpenSSLAsymmetricKey)) {
    echo "Private Key NOK" . PHP_EOL;
    exit(1);
}
echo "Private Key OK" . PHP_EOL;

$key = openssl_pkey_get_public(sprintf("pkcs11:object=VJPubKey;type=public"));
if (!($key instanceof OpenSSLAsymmetricKey)) {
    echo "Public Key NOK" . PHP_EOL;
    exit(1);
}
echo "Public Key OK" . PHP_EOL;

$cert = openssl_x509_read(sprintf("pkcs11:object=VJCert;type=cert"));
if (!($cert instanceof OpenSSLCertificate)) {
    echo "Cert NOK" . PHP_EOL;
    exit(1);
}
echo "Cert OK" . PHP_EOL;
$certArray=openssl_x509_parse($cert);

if ($certArray['name'] !== "/CN=MyCertVJ") {
    echo "Cert content NOK" . PHP_EOL;
    exit(1);
}
echo "Cert content OK" . PHP_EOL;

?>
--EXPECT--
Private Key OK
Public Key OK
Cert OK
Cert content OK
