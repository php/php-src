<?php

/** @generate-function-entries */

function use_soap_error_handler(bool $handler = true): bool {}

function is_soap_fault($object): bool {}

class SoapParam
{
    public function __construct($data, string $name) {}
}

class SoapHeader
{
    public function __construct(string $namespace, string $name, $data = UNKNOWN, bool $mustunderstand = false, $actor = UNKNOWN) {}
}

class SoapFault extends Exception
{
    public function __construct($faultcode, string $faultstring, ?string $faultactor = null, $detail = null, ?string $faultname = null, $headerfault = null) {}

    public function __toString(): string {}
}

class SoapVar
{
    public function __construct($data, $encoding, string $type_name = "", string $type_namespace = "", string $node_name = "", string $node_namespace = "") {}
}

class SoapServer
{
    public function __construct($wsdl, array $options = []) {}

    /** @return void */
    public function fault(string $code, string $string, string $actor = "", $details = null, string $name = "") {}

    /** @return void */
    public function addSoapHeader(SoapHeader $object) {}

    /** @return void */
    public function setPersistence(int $mode) {}

    /** @return void */
    public function setClass(string $class_name, ...$argv) {}

    /** @return void */
    public function setObject(object $object) {}

    /** @return array */
    public function getFunctions() {}

    /** @return void */
    public function addFunction($functions) {}

    /** @return void */
    public function handle(string $soap_request = UNKNOWN) {}
}

class SoapClient
{
    public function __construct($wsdl, array $options = []) {}

    /** @return mixed */
    public function __call(string $function_name, array $arguments) {}

    /**
     * @return mixed
     * @alias SoapClient::__call
     */
    public function __soapCall(string $function_name, array $arguments, ?array $options = null, $input_headers = null, $output_headers = null) {}

    /** @return array|null */
    public function __getFunctions() {}

    /** @return array|null */
    public function __getTypes() {}

    /** @return string|null */
    public function __getLastRequest() {}

    /** @return string|null */
    public function __getLastResponse() {}

    /** @return string|null */
    public function __getLastRequestHeaders() {}

    /** @return string|null */
    public function __getLastResponseHeaders() {}

    /** @return string|null */
    public function __doRequest(string $request, string $location, string $action, int $version, int $one_way = 0) {}

    /** @return void */
    public function __setCookie(string $name, ?string $value = null) {}

    /** @return array */
    public function __getCookies() {}

    /** @return bool */
    public function __setSoapHeaders($soapheaders = null) {}

    /** @return string|null */
    public function __setLocation(string $new_location = "") {}
}
