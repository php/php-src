--TEST--
bool openssl_pkcs12_export(mixed $x509 , string &$out , mixed $priv_key , string $pass [, array $args ]);
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
$priv_key_file_pem = "file://" . dirname(__FILE__) . "/{$key_file}";
$priv_key_dir_pem = __DIR__ . "/{$pem_file}";
$pass_phrase = "JavaIsBetterThanPython:-)";
$args = array(
    'extracerts' => $priv_key_file_pem,
    'friendly_name' => 'My signed cert by CA certificate'
);

try{
    $cert = openssl_x509_read(file_get_contents($priv_key_dir_pem));
    
    if ( false !== $cert ){
        $priv_key = openssl_pkey_get_private($priv_key_file_pem);
        
        if( false !== $priv_key ){
        
            if (openssl_pkcs12_export($cert, $pkcs12_out, $priv_key, $pass_phrase, $args)) {
                
                if (null !== $pkcs12_out) {
                    print("okey");
                    
                } else {
                    print("openssl has failure to return pkcs12");
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
