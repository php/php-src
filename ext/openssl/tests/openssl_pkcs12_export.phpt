--TEST--
bool openssl_pkcs12_export ( mixed $x509 , string &$out , mixed $priv_key , string $pass [, array $args ] );
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
$privkeyFilePem = "file://" . dirname(__FILE__) . "/{$keyFile}";
$privkeyDirPem = __DIR__ . "/{$pemFile}";
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
        
            if (openssl_pkcs12_export($cert, $pkcs12Out, $privkey, $passPhrase, $args)) {
                
                if (null !== $pkcs12Out) {
                    print("okey");
                    
                } else {
                    print("okey");
                }        
                
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
