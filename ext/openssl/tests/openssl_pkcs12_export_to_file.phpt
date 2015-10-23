--TEST--
bool openssl_pkcs12_export_to_file ( mixed $x509 , string $filename , mixed $priv_key , string $pass [, array $args ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip";
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
$pemFile = "bug37820cert.pem";
$keyFile = "bug37820key.pem";
$pkcs12Export = "openssl_pkcs12_export_to_file.pem";
$privkeyFilePem = "file://" . dirname(__FILE__) . "/{$keyFile}";
$privkeyDirPem = __DIR__ . "/{$pemFile}";
$exportFile = __DIR__ . "/{$pkcs12Export}";
$passPhrase = "JavaIsBetterThanPython:-)";
$args = array(
    'extracerts' => $privkeyFilePem,
    'friendly_name' => 'My signed cert by CA certificate'
);

try{
    $cert = openssl_x509_read(file_get_contents($privkeyDirPem));
    
    if ( false !== $cert ){
        $privkey = openssl_pkey_get_private($privkeyFilePem);
        
        if( false !== $privkey ){
        
            if (openssl_pkcs12_export_to_file($cert, $exportFile, $privkey, $passPhrase, $args)) {
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
    
} catch(Exception $ex){
    print($ex->getMessage());
}
?>
--EXPECT--
okey
