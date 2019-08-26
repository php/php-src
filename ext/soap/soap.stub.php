<?php

function use_soap_error_handler(bool $handler = true): bool {}

function is_soap_fault($object): bool {}

class SoapParam
{
    function __construct($data, string $name);
}

class SoapHeader
{
    function __construct(string $namespace, string $name, $data = UNKNOWN, bool $mustunderstand = false, $actor = UNKNOWN);
}

class SoapFault extends Exception
{
    function __construct($faultcode, string $faultstring, ?string $faultactor = null, $detail = null, ?string $faultname = null, $headerfault = null);

    /** @return string */
    function __toString();
}

class SoapVar
{
    function __construct($data, $encoding, string $type_name = "", string $type_namespace = "", string $node_name = "", string $node_namespace = "");
}

class SoapServer
{
    function __construct($wsdl, array $options = []);

    /** @return void */
    function fault(string $code, string $string, string $actor = "", $details = null, string $name = "");

    /** @return void */
    function addSoapHeader(SoapHeader $object);

    /** @return void */
    function setPersistence(int $mode);

    /** @return void */
    function setClass(string $class_name, ...$argv);

    /** @return void */
    function setObject(object $object);

    /** @return array */
    function getFunctions();

    /** @return void */
    function addFunction($functions);

    /** @return void */
    function handle(string $soap_request = UNKNOWN);
}

class SoapClient
{
    function __construct($wsdl, array $options = []);

    /** @return mixed */
    function __call(string $function_name, array $arguments);

    /** @return mixed */
    function __soapCall(string $function_name, array $arguments, ?array $options = null, $input_headers = null, $output_headers = null);

    /** @return array|null */
    function __getFunctions();

    /** @return array|null */
    function __getTypes();

    /** @return ?string */
    function __getLastRequest();

    /** @return ?string */
    function __getLastResponse();

    /** @return ?string */
    function __getLastRequestHeaders();

    /** @return ?string */
    function __getLastResponseHeaders();

    /** @return ?string */
    function __doRequest(string $request, string $location, string $action, int $version, int $one_way = 0);

    /** @return void */
    function __setCookie(string $name, ?string $value = null);

    /** @return array */
    function __getCookies();

    /** @return bool */
    function __setSoapHeaders($soapheaders = null);

    /** @return ?string */
    function __setLocation(string $new_location = "");
}
