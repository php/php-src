--TEST--
openssl_cms_encrypt() der test
--EXTENSIONS--
openssl
--FILE--
<?php
$infile = __DIR__ . "/plain.txt";
$tname = tempnam(sys_get_temp_dir(), "ssl");
if ($tname === false)
    die("failed to get a temporary filename!");
$cryptfile= $tname . ".der";
$decryptfile = $tname . ".out";
$single_cert = "file://" . __DIR__ . "/cert.crt";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$headers = array("test@test", "testing openssl_cms_encrypt()");
$cipher = OPENSSL_CIPHER_AES_128_CBC;

var_dump(openssl_cms_encrypt($infile, $cryptfile, $single_cert, $headers, OPENSSL_CMS_BINARY, OPENSSL_ENCODING_DER, $cipher));
if (openssl_cms_decrypt($cryptfile, $decryptfile, $single_cert, $privkey, OPENSSL_ENCODING_DER) == false) {
    print "DER decrypt error\n";
    print "recipient:\n";
    readfile($single_cert);
    print "input:\n";
    readfile($infile);
    $der=file_get_contents($cryptfile);
    print "outfile base64:\n" . base64_encode($der) . "\n--------\n";
    while (( $errstr=openssl_error_string()) != false) {
        print $errstr . "\n";
    }
} else {
    readfile($decryptfile);
}

if (file_exists($cryptfile)) {
    echo "true\n";
    unlink($cryptfile);
}
if (file_exists($decryptfile)) {
    echo "true\n";
    unlink($decryptfile);
}
unlink($tname);
?>
--EXPECT--
bool(true)
Now is the winter of our discontent.
true
true

