--TEST--
SoapClient reports encoding and decoding errors as SoapFault without a bailout
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

const ENC11 = 'http://schemas.xmlsoap.org/soap/encoding/';
const ENC12 = 'http://www.w3.org/2003/05/soap-encoding';

class TypedNeed
{
    public int $x;
}

enum Plain
{
    case A;
}

function no_ctor(string $class): object
{
    return (new ReflectionClass($class))->newInstanceWithoutConstructor();
}

$big = '<x id="r" xsi:type="SOAP-ENC:Array" SOAP-ENC:arrayType="xsd:anyType[2]"><i href="#r"/><i href="#r"/></x>';

class Client extends SoapClient
{
    public $response;
    public function __doRequest($request, $location, $action, $version, $oneWay = false, ?string $uriParserClass = null): ?string
    {
        return $this->response;
    }
}

function envelope(string $body, string $header = ''): string
{
    return '<?xml version="1.0"?><SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"'
        . ' xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema"'
        . ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:apache="http://xml.apache.org/xml-soap"'
        . ' xmlns:ns1="urn:t">' . $header . '<SOAP-ENV:Body>' . $body . '</SOAP-ENV:Body></SOAP-ENV:Envelope>';
}

$wsdl = __DIR__ . '/encoding_errors_no_bailout.wsdl';
file_put_contents($wsdl, <<<'WSDL'
<?xml version="1.0"?>
<definitions xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:tns="urn:t" targetNamespace="urn:t">
<types><xsd:schema targetNamespace="urn:t">
<xsd:complexType name="Fixed1"><xsd:sequence><xsd:element name="f" type="xsd:string" fixed="abc"/></xsd:sequence></xsd:complexType>
<xsd:complexType name="FixedN"><xsd:sequence><xsd:element name="f" type="xsd:string" fixed="abc" maxOccurs="unbounded"/></xsd:sequence></xsd:complexType>
<xsd:complexType name="FixedAttr"><xsd:sequence/><xsd:attribute name="a" type="xsd:string" fixed="x"/></xsd:complexType>
<xsd:complexType name="Need"><xsd:sequence><xsd:element name="x" type="xsd:int"/></xsd:sequence></xsd:complexType>
<xsd:complexType name="NeedAny"><xsd:sequence><xsd:any minOccurs="1" processContents="lax"/></xsd:sequence></xsd:complexType>
<xsd:simpleType name="IntList"><xsd:list itemType="xsd:int"/></xsd:simpleType>
</xsd:schema></types>
<message name="inFixed1"><part name="p" type="tns:Fixed1"/></message><message name="outFixed1"><part name="return" type="tns:Fixed1"/></message>
<message name="inFixedN"><part name="p" type="tns:FixedN"/></message><message name="outFixedN"><part name="return" type="tns:FixedN"/></message>
<message name="inFixedAttr"><part name="p" type="tns:FixedAttr"/></message><message name="outFixedAttr"><part name="return" type="tns:FixedAttr"/></message>
<message name="inNeed"><part name="p" type="tns:Need"/></message><message name="outNeed"><part name="return" type="tns:Need"/></message>
<message name="inNeedAny"><part name="p" type="tns:NeedAny"/></message><message name="outNeedAny"><part name="return" type="tns:NeedAny"/></message>
<message name="inIntList"><part name="p" type="tns:IntList"/></message><message name="outIntList"><part name="return" type="tns:IntList"/></message>
<portType name="PT"><operation name="opFixed1"><input message="tns:inFixed1"/><output message="tns:outFixed1"/></operation><operation name="opFixedN"><input message="tns:inFixedN"/><output message="tns:outFixedN"/></operation><operation name="opFixedAttr"><input message="tns:inFixedAttr"/><output message="tns:outFixedAttr"/></operation><operation name="opNeed"><input message="tns:inNeed"/><output message="tns:outNeed"/></operation><operation name="opNeedAny"><input message="tns:inNeedAny"/><output message="tns:outNeedAny"/></operation><operation name="opIntList"><input message="tns:inIntList"/><output message="tns:outIntList"/></operation></portType>
<binding name="B" type="tns:PT"><soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/><operation name="opFixed1"><soap:operation soapAction="opFixed1"/><input><soap:body use="literal" namespace="urn:t"/></input><output><soap:body use="literal" namespace="urn:t"/></output></operation><operation name="opFixedN"><soap:operation soapAction="opFixedN"/><input><soap:body use="literal" namespace="urn:t"/></input><output><soap:body use="literal" namespace="urn:t"/></output></operation><operation name="opFixedAttr"><soap:operation soapAction="opFixedAttr"/><input><soap:body use="literal" namespace="urn:t"/></input><output><soap:body use="literal" namespace="urn:t"/></output></operation><operation name="opNeed"><soap:operation soapAction="opNeed"/><input><soap:body use="literal" namespace="urn:t"/></input><output><soap:body use="literal" namespace="urn:t"/></output></operation><operation name="opNeedAny"><soap:operation soapAction="opNeedAny"/><input><soap:body use="literal" namespace="urn:t"/></input><output><soap:body use="literal" namespace="urn:t"/></output></operation><operation name="opIntList"><soap:operation soapAction="opIntList"/><input><soap:body use="literal" namespace="urn:t"/></input><output><soap:body use="literal" namespace="urn:t"/></output></operation></binding>
<service name="S"><port name="P" binding="tns:B"><soap:address location="test://"/></port></service></definitions>
WSDL);

$cases = [

    'dec_string_elem' => ['value' => '<return xsi:type="xsd:string"><a/></return>'],
    'dec_normalized_elem' => ['value' => '<return xsi:type="xsd:normalizedString"><a/></return>'],
    'dec_token_elem' => ['value' => '<return xsi:type="xsd:token"><a/></return>'],
    'dec_base64_elem' => ['value' => '<return xsi:type="xsd:base64Binary"><a/></return>'],
    'dec_hexbin_elem' => ['value' => '<return xsi:type="xsd:hexBinary"><a/></return>'],
    'dec_double_elem' => ['value' => '<return xsi:type="xsd:double"><a/></return>'],
    'dec_long_elem' => ['value' => '<return xsi:type="xsd:int"><a/></return>'],
    'dec_bool_elem' => ['value' => '<return xsi:type="xsd:boolean"><a/></return>'],
    'dec_hexbin_odd' => ['value' => '<return xsi:type="xsd:hexBinary">ABC</return>'],
    'dec_hexbin_high' => ['value' => '<return xsi:type="xsd:hexBinary">ZA</return>'],
    'dec_hexbin_low' => ['value' => '<return xsi:type="xsd:hexBinary">AZ</return>'],
    'dec_double_invalid' => ['value' => '<return xsi:type="xsd:double">abc</return>'],
    'dec_long_invalid' => ['value' => '<return xsi:type="xsd:int">abc</return>'],
    'dec_two_values_first_wins' => ['value' => '<return><a xsi:type="xsd:int">abc</a><b xsi:type="xsd:hexBinary">ABC</b></return>'],
    'dec_map_no_key' => ['value' => '<return xsi:type="apache:Map"><item><value>1</value></item></return>'],
    'dec_map_no_value' => ['value' => '<return xsi:type="apache:Map"><item><key>k</key></item></return>'],
    'dec_map_bad_key' => ['value' => '<return xsi:type="apache:Map"><item><key><a>1</a></key><value>1</value></item></return>'],
    'dec_array_star' => ['value' => '<return xsi:type="SOAP-ENC:Array" xmlns:e12="' . ENC12 . '" e12:itemType="xsd:string" e12:arraySize="2 * 1"><i>a</i></return>'],
    'dec_array_position_digits' => ['value' => '<return xsi:type="SOAP-ENC:Array" SOAP-ENC:arrayType="xsd:string[1]"><i SOAP-ENC:position="[99999999999]">a</i></return>'],
    'dec_array_position_increment' => ['value' => '<return xsi:type="SOAP-ENC:Array" SOAP-ENC:arrayType="xsd:string[1]" SOAP-ENC:offset="[2147483647]"><i>a</i><i>b</i></return>'],
    'dec_href_unresolved' => ['value' => '<return href="#nope"/>'],
    'dec_href_external' => ['value' => '<return href="http://example.org/x"/>'],
    'dec_ref12_unresolved' => ['value' => '<return xmlns:e12="' . ENC12 . '" e12:ref="nope"/>'],
    'dec_ref12_self' => ['value' => '<return xmlns:e12="' . ENC12 . '" e12:id="me" e12:ref="me"/>'],
    'dec_fault_string' => ['body' => '<SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring><a/><b/></faultstring></SOAP-ENV:Fault>'],
    'dec_header' => ['header' => '<SOAP-ENV:Header><h1 xsi:type="xsd:int">1</h1><h2 xsi:type="xsd:int">abc</h2></SOAP-ENV:Header>', 'value' => '<return xsi:type="xsd:string">abc</return>'],
    'dec_classmap_typed' => ['wsdl' => 'Need', 'value' => '<return><x>abc</x></return>', 'opts' => ['classmap' => ['Need' => 'TypedNeed']]],
    'enc_enum_after_error' => ['args' => fn() => [no_ctor(SoapVar::class), Plain::A]],
    'dec_fixed_element' => ['wsdl' => 'Fixed1', 'value' => '<return><f>zzz</f></return>'],
    'dec_fixed_element_repeated' => ['wsdl' => 'FixedN', 'value' => '<return><f>abc</f><f>zzz</f></return>'],
    'dec_fixed_attribute' => ['wsdl' => 'FixedAttr', 'value' => '<return a="zzz"/>'],

    'enc_soapvar_no_enc_type' => ['args' => fn() => [no_ctor(SoapVar::class)]],
    'enc_soapvar_no_enc_type_in_array' => ['args' => fn() => [[no_ctor(SoapVar::class), no_ctor(SoapVar::class)]]],
    'enc_soapvar_unknown_type' => ['args' => function () { $v = no_ctor(SoapVar::class); $v->enc_type = 99999; $v->enc_value = 1; return [$v]; }],
    'enc_resource' => ['args' => fn() => [fopen('php://memory', 'r')]],
    'enc_resource_in_object' => ['args' => fn() => [(object) ['a' => 1, 'r' => fopen('php://memory', 'r'), 'b' => 2]]],
    'enc_invalid_utf8' => ['args' => fn() => ["ab\xffcd"]],
    'enc_invalid_utf8_twice' => ['args' => fn() => ["ab\xffcd", "\xfe"]],
    'enc_invalid_datetime' => ['args' => fn() => [new SoapVar(no_ctor(DateTime::class), XSD_DATETIME)]],
    'enc_fixed_element' => ['wsdl' => 'Fixed1', 'args' => fn() => [(object) ['f' => 'zzz']]],
    'enc_fixed_element_array' => ['wsdl' => 'FixedN', 'args' => fn() => [(object) ['f' => ['abc', 'zzz']]]],
    'enc_fixed_attribute' => ['wsdl' => 'FixedAttr', 'args' => fn() => [(object) ['a' => 'zzz']]],
    'enc_missing_property' => ['wsdl' => 'Need', 'args' => fn() => [(object) ['y' => 1]]],
    'enc_missing_any' => ['wsdl' => 'NeedAny', 'args' => fn() => [(object) []]],
    'enc_list_item' => ['wsdl' => 'IntList', 'args' => fn() => [[1, null, 3]]],
];

foreach ($cases as $name => $case) {
    $fn = isset($case['wsdl']) ? 'op' . $case['wsdl'] : 'test';
    $opts = ($case['opts'] ?? []) + (isset($case['wsdl']) ? [] : ['location' => 'test://', 'uri' => 'urn:t']);
    $client = new Client(isset($case['wsdl']) ? $wsdl : null, $opts + ['trace' => true]);
    $value = $case['value'] ?? null;
    $body = $case['body'] ?? ($value ? "<ns1:{$fn}Response>$value</ns1:{$fn}Response>" : "<ns1:{$fn}Response/>");
    $client->response = envelope($body, $case['header'] ?? '');
    $headers = [];
    try {
        $client->__soapCall($fn, isset($case['args']) ? ($case['args'])() : [], null, null, $headers);
        echo "$name: no error\n";
    } catch (SoapFault $e) {
        echo "$name: $e->faultcode: $e->faultstring\n";
        $fault = (new ReflectionProperty(SoapClient::class, '__soap_fault'))->getValue($client);
        if ($fault !== $e) {
            echo "  __soap_fault is not the thrown SoapFault\n";
        }
        if ($e->getPrevious()) {
            echo "  previous: ", get_class($e->getPrevious()), "\n";
        }
        if (isset($case['args']) && $client->__getLastRequest() !== null) {
            echo "  the request was sent\n";
        }
        if ($headers) {
            echo "  output headers: ", implode(', ', array_keys($headers)), "\n";
        }
    }
}
var_dump(gc_status()['protected']);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/encoding_errors_no_bailout.wsdl');
?>
--EXPECT--
dec_string_elem: Client: SOAP-ERROR: Encoding: Type 'string' value must contain a single text or CDATA node
dec_normalized_elem: Client: SOAP-ERROR: Encoding: Type 'normalizedString' value must contain a single text or CDATA node
dec_token_elem: Client: SOAP-ERROR: Encoding: Type 'token' value must contain a single text or CDATA node
dec_base64_elem: Client: SOAP-ERROR: Encoding: Type 'base64Binary' value must contain a single text or CDATA node
dec_hexbin_elem: Client: SOAP-ERROR: Encoding: Type 'hexBinary' value must contain a single text or CDATA node
dec_double_elem: Client: SOAP-ERROR: Encoding: Type 'double' value must contain a single text or CDATA node
dec_long_elem: Client: SOAP-ERROR: Encoding: Type 'int' value must contain a single text or CDATA node
dec_bool_elem: Client: SOAP-ERROR: Encoding: Type 'boolean' value must contain a single text or CDATA node
dec_hexbin_odd: Client: SOAP-ERROR: Encoding: Type 'hexBinary' value must contain an even number of hexadecimal digits
dec_hexbin_high: Client: SOAP-ERROR: Encoding: Invalid value for type 'hexBinary'
dec_hexbin_low: Client: SOAP-ERROR: Encoding: Invalid value for type 'hexBinary'
dec_double_invalid: Client: SOAP-ERROR: Encoding: Invalid value for type 'double'
dec_long_invalid: Client: SOAP-ERROR: Encoding: Invalid value for type 'int'
dec_two_values_first_wins: Client: SOAP-ERROR: Encoding: Invalid value for type 'int'
dec_map_no_key: Client: SOAP-ERROR: Encoding: Can't decode apache map, missing key
dec_map_no_value: Client: SOAP-ERROR: Encoding: Can't decode apache map, missing value
dec_map_bad_key: Client: SOAP-ERROR: Encoding: Can't decode apache map, only Strings or Longs are allowed as keys
dec_array_star: Client: SOAP-ERROR: Encoding: '*' may only be first arraySize value in list
dec_array_position_digits: Client: SOAP-ERROR: Encoding: array index out of range
dec_array_position_increment: Client: SOAP-ERROR: Encoding: array index out of range
dec_href_unresolved: Client: SOAP-ERROR: Encoding: Unresolved reference '#nope'
dec_href_external: Client: SOAP-ERROR: Encoding: External reference 'http://example.org/x'
dec_ref12_unresolved: Client: SOAP-ERROR: Encoding: Unresolved reference 'nope'
dec_ref12_self: Client: SOAP-ERROR: Encoding: Violation of id and ref information items 'me'
dec_fault_string: Client: SOAP-ERROR: Encoding: Type 'string' value must contain a single text or CDATA node
dec_header: Client: SOAP-ERROR: Encoding: Invalid value for type 'int'
  output headers: h1
dec_classmap_typed: Client: SOAP-ERROR: Encoding: Invalid value for type 'int'
enc_enum_after_error: Client: SOAP-ERROR: Encoding: SoapVar has no 'enc_type' property
dec_fixed_element: Client: SOAP-ERROR: Encoding: Element 'f' has fixed value 'abc' (value 'zzz' is not allowed)
dec_fixed_element_repeated: Client: SOAP-ERROR: Encoding: Element 'f' has fixed value 'abc' (value 'zzz' is not allowed)
dec_fixed_attribute: Client: SOAP-ERROR: Encoding: Attribute 'a' has fixed value 'x' (value 'zzz' is not allowed)
enc_soapvar_no_enc_type: Client: SOAP-ERROR: Encoding: SoapVar has no 'enc_type' property
enc_soapvar_no_enc_type_in_array: Client: SOAP-ERROR: Encoding: SoapVar has no 'enc_type' property
enc_soapvar_unknown_type: Client: SOAP-ERROR: Encoding: Cannot find encoding
enc_resource: Client: SOAP-ERROR: Encoding: Cannot find encoding
enc_resource_in_object: Client: SOAP-ERROR: Encoding: Cannot find encoding
enc_invalid_utf8: Client: SOAP-ERROR: Encoding: string 'ab\xff...' is not a valid utf-8 string
enc_invalid_utf8_twice: Client: SOAP-ERROR: Encoding: string 'ab\xff...' is not a valid utf-8 string
enc_invalid_datetime: Client: SOAP-ERROR: Encoding: Invalid DateTimeInterface
enc_fixed_element: Client: SOAP-ERROR: Encoding: Element 'f' has fixed value 'abc' (value 'zzz' is not allowed)
enc_fixed_element_array: Client: SOAP-ERROR: Encoding: Element 'f' has fixed value 'abc' (value 'zzz' is not allowed)
enc_fixed_attribute: Client: SOAP-ERROR: Encoding: Attribute 'a' has fixed value 'x' (value 'zzz' is not allowed)
enc_missing_property: Client: SOAP-ERROR: Encoding: object has no 'x' property
enc_missing_any: Client: SOAP-ERROR: Encoding: object has no 'any' property
enc_list_item: Client: SOAP-ERROR: Encoding: Failed to encode list item of type 'int' for list type 'IntList'
bool(false)
