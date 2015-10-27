--TEST--
bool openssl_pkcs12_export_to_file ( mixed $x509 , string $filename , mixed $priv_key , string $pass [, array $args ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if (!extension_loaded("openssl")) print "skip";
?>
--FILE--
<?php
$pem_file = "bug37820cert.pem";
$key_file = "bug37820key.pem";
$pkcs12_export = "openssl_pkcs12_export_to_file.pem";
$priv_key_file_pem = "file://" . dirname(__FILE__) . "/{$key_file}";
$priv_key_dir_pem = __DIR__ . "/{$pem_file}";
$export_file = __DIR__ . "/{$pkcs12_export}";
$pass_phrase = "JavaIsBetterThanPython:-)";
$args = array(
    'extracerts' => $priv_key_file_pem,
    'friendly_name' => 'My signed cert by CA certificate'
);

$cert = openssl_x509_read(file_get_contents($priv_key_dir_pem));

if ( false !== $cert ){
    $priv_key = openssl_pkey_get_private($priv_key_file_pem);
    
    if( false !== $priv_key ){
    
        if (openssl_pkcs12_export_to_file($cert, $export_file, $priv_key, $pass_phrase, $args)) {
            print("okey");
            
        } else {
            print("openssl has failure to export pkcs12");
        }
        
    } else {
        print("openssl could not read key file");
    }
    
} else {
    print("openssl x509 could not read pem file");
}
?>
--CLEAN--
<?php
unlink(__DIR__."/openssl_pkcs12_export_to_file.pem");
?>
--EXPECT--
okey
