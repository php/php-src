--TEST--
Bug #62900 (Wrong namespace on xsd import error message)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

$wsdl_with_ns = <<<XML
<definitions xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:xs="http://www.w3.org/2001/XMLSchema" targetNamespace="http://test-uri">
	<types>
		<xs:schema targetNamespace="http://test-uri" elementFormDefault="qualified">
			<import namespace="http://www.w3.org/XML/1998/namespace" schemaLocation="bug62900.xsd" />
		</xs:schema>
	</types>
</definitions>
XML;

$wsdl_without_ns = <<<XML
<definitions xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:xs="http://www.w3.org/2001/XMLSchema" targetNamespace="http://test-uri">
	<types>
		<xs:schema elementFormDefault="qualified">
			<import schemaLocation="bug62900.xsd" />
		</xs:schema>
	</types>
</definitions>
XML;

$xsd_with_wrong_ns = <<<XML
<!DOCTYPE xs:schema PUBLIC "-//W3C//DTD XMLSCHEMA 200102//EN" "XMLSchema.dtd">
<xs:schema targetNamespace="http://www.w3.org/XML/1998/namespacex" xmlns:xs="http://www.w3.org/2001/XMLSchema" xml:lang="en"/>
XML;

$xsd_without_ns = <<<XML
<!DOCTYPE xs:schema PUBLIC "-//W3C//DTD XMLSCHEMA 200102//EN" "XMLSchema.dtd">
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xml:lang="en"/>
XML;

$combinations = [
    [$wsdl_with_ns, $xsd_with_wrong_ns],
    [$wsdl_with_ns, $xsd_without_ns],
    [$wsdl_without_ns, $xsd_with_wrong_ns],
    [$wsdl_without_ns, $xsd_without_ns],
];

chdir(__DIR__);

$args = ["-d", "display_startup_errors=0", "-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "soap." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
    // Necessary such that it works from a development directory in which case extension_dir might not be the real extension dir
    $args[] = "-c";
    $args[] = php_ini_loaded_file();
}

foreach ($combinations as list($wsdl, $xsd)) {
    file_put_contents(__DIR__."/bug62900.wsdl", $wsdl);
    file_put_contents(__DIR__."/bug62900.xsd", $xsd);

    $proc = proc_open([PHP_BINARY, ...$args, __DIR__.'/bug62900_run'], [1 => ["pipe", "w"], 2 => ["pipe", "w"]], $pipes);
    echo stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    proc_close($proc);
}

?>
--CLEAN--
<?php
@unlink(__DIR__."/bug62900.wsdl");
@unlink(__DIR__."/bug62900.xsd");
?>
--EXPECTF--
Fatal error: Uncaught SoapFault exception: [WSDL] SOAP-ERROR: Parsing Schema: can't import schema from '%sbug62900.xsd', unexpected 'targetNamespace'='http://www.w3.org/XML/1998/namespacex', expected 'http://www.w3.org/XML/1998/namespace' in %s:%d
Stack trace:
#0 %s(%d): SoapClient->__construct(%s)
#1 {main}
  thrown in %s on line %d

Fatal error: Uncaught SoapFault exception: [WSDL] SOAP-ERROR: Parsing Schema: can't import schema from '%sbug62900.xsd', missing 'targetNamespace', expected 'http://www.w3.org/XML/1998/namespace' in %s:%d
Stack trace:
#0 %s(%d): SoapClient->__construct(%s)
#1 {main}
  thrown in %s on line %d

Fatal error: Uncaught SoapFault exception: [WSDL] SOAP-ERROR: Parsing Schema: can't import schema from '%sbug62900.xsd', unexpected 'targetNamespace'='http://www.w3.org/XML/1998/namespacex', expected no 'targetNamespace' in %s:%d
Stack trace:
#0 %s(%d): SoapClient->__construct(%s)
#1 {main}
  thrown in %s on line %d

Fatal error: Uncaught SoapFault exception: [WSDL] SOAP-ERROR: Parsing WSDL: Couldn't bind to service in %s:%d
Stack trace:
#0 %s(%d): SoapClient->__construct(%s)
#1 {main}
  thrown in %s on line %d
