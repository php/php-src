--TEST--
WSDL cache corruption when soap:header has headerfaults
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=1
--FILE--
<?php
$dir = __DIR__ . '/headerfault_cache_dir';
@mkdir($dir);
ini_set('soap.wsdl_cache_dir', $dir);

$options = ['cache_wsdl' => WSDL_CACHE_DISK];

$c1 = new SoapClient(__DIR__ . '/headerfault_cache.wsdl', $options);
var_dump($c1->__getFunctions());

$c2 = new SoapClient(__DIR__ . '/headerfault_cache.wsdl', $options);
var_dump($c2->__getFunctions());

echo "ok\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/headerfault_cache_dir';
if (is_dir($dir)) {
    foreach (scandir($dir) as $f) {
        if ($f === '.' || $f === '..') {
            continue;
        }
        @unlink($dir . '/' . $f);
    }
    @rmdir($dir);
}
?>
--EXPECT--
array(1) {
  [0]=>
  string(32) "string testHeader(string $param)"
}
array(1) {
  [0]=>
  string(32) "string testHeader(string $param)"
}
ok
