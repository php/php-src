--TEST--
Bug #75306 (Memleak in SoapClient)
--EXTENSIONS--
soap
--FILE--
<?php
$options = array("cache_wsdl" => WSDL_CACHE_NONE);
// Need a warm-up for globals
for ($i = 0; $i < 10; $i++) {
    $client = new SoapClient("ext/soap/tests/test.wsdl", $options);
}
$usage = memory_get_usage();
for ($i = 0; $i < 10; $i++) {
    $client = new SoapClient("ext/soap/tests/test.wsdl", $options);
}
$usage_delta = memory_get_usage() - $usage;
var_dump($usage_delta);
?>
--EXPECT--
int(0)
