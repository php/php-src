<?php

/** @generate-function-entries */

function use_soap_error_handler(bool $handler = true): bool {}

function is_soap_fault(mixed $object): bool {}

class SoapParam
{
    public function __construct(mixed $data, string $name) {}
}

class SoapHeader
{
    public function __construct(string $namespace, string $name, mixed $data = UNKNOWN, bool $mustunderstand = false, string|int|null $actor = null) {}
}

class SoapFault extends Exception
{
    public function __construct(array|string|null $faultcode, string $faultstring, ?string $faultactor = null, mixed $detail = null, ?string $faultname = null, mixed $headerfault = null) {}

    public function __toString(): string {}
}

class SoapVar
{
    public function __construct(mixed $data, ?int $encoding, string $type_name = "", string $type_namespace = "", string $node_name = "", string $node_namespace = "") {}
}

class SoapServer
{
    public function __construct(?string $wsdl, array $options = []) {}

    /** @return void */
    public function fault(string $code, string $string, string $actor = "", mixed $details = null, string $name = "") {}

    /** @return void */
    public function addSoapHeader(SoapHeader $object) {}

    /** @return void */
    public function setPersistence(int $mode) {}

    /** @return void */
    public function setClass(string $class_name, mixed ...$argv) {}

    /** @return void */
    public function setObject(object $object) {}

    /** @return array */
    public function getFunctions() {}

    /**
     * @param array|string|int $functions
     * @return void
     */
    public function addFunction($functions) {}

    /** @return void */
    public function handle(string $soap_request = UNKNOWN) {}
}

class SoapClient
{
    public function __construct(?string $wsdl, array $options = []) {}

    /** @return mixed */
    public function __call(string $function_name, array $arguments) {}

    /**
     * @param SoapHeader|array|null $input_headers
     * @param array $output_headers
     * @return mixed
     */
    public function __soapCall(string $function_name, array $arguments, ?array $options = null, $input_headers = null, &$output_headers = null) {}

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

    /**
     * @param SoapHeader|array|null $soapheaders
     * @return bool
     */
    public function __setSoapHeaders($soapheaders = null) {}

    /** @return string|null */
    public function __setLocation(string $new_location = "") {}
}
