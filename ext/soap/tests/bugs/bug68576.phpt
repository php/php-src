--TEST--
Bug #68576 (Namespaces in schema php_schema.c)
--EXTENSIONS--
soap
--FILE--
<?php
try {
    $client = new SoapClient(__DIR__.'/bug68576.wsdl', ['cache_wsdl' => WSDL_CACHE_NONE]);
} catch (SoapFault $e) {
    // Should throw a SoapFault for the binding to the service, not for a schema error
    echo get_class($e)."\n";
    echo $e->faultstring."\n";
}
?>
--EXPECT--
SoapFault
SoapClient::__construct(): Parsing WSDL: Couldn't bind to service
