--TEST--
SoapClient reports WSDL and schema errors as SoapFault without a bailout
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
function make_wsdl(array $parts): string {
    $parts += [
        'types' => '<types><xsd:schema targetNamespace="urn:t">{schema}</xsd:schema></types>',
        'schema' => '<xsd:complexType name="T"><xsd:sequence><xsd:element name="a" type="xsd:string"/></xsd:sequence></xsd:complexType>',
        'messages' => '<message name="Req"><part name="a" type="tns:T"/></message><message name="Resp"><part name="r" type="xsd:string"/></message>',
        'port_type' => '<portType name="PT"><operation name="op"><input message="tns:Req"/><output message="tns:Resp"/>{pt_extra}</operation></portType>',
        'pt_extra' => '',
        'binding' => '<binding name="B" type="tns:PT"><soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>{b_extra}<operation name="op"><soap:operation soapAction="urn:op"/>{op_extra}<input>{in_body}</input><output>{out_body}</output>{b_fault}</operation></binding>',
        'b_extra' => '',
        'op_extra' => '',
        'in_body' => '<soap:body use="encoded" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>',
        'out_body' => '<soap:body use="encoded" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>',
        'b_fault' => '',
        'service' => '<service name="S"><port name="P" binding="tns:B">{port_body}</port>{svc_extra}</service>',
        'port_body' => '<soap:address location="http://127.0.0.1:1/x"/>',
        'svc_extra' => '',
        'extra' => '',
    ];
    $pairs = [];
    foreach ($parts as $k => $v) {
        $pairs['{' . $k . '}'] = $v;
    }
    // Placeholders can nest up to three levels deep
    $body = '{types}{messages}{port_type}{binding}{service}{extra}';
    for ($i = 0; $i < 4; $i++) {
        $body = strtr($body, $pairs);
    }
    return '<?xml version="1.0"?>'
        . '<definitions xmlns="http://schemas.xmlsoap.org/wsdl/" xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/"'
        . ' xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns:xsd="http://www.w3.org/2001/XMLSchema"'
        . ' xmlns:tns="urn:t" xmlns:x="urn:ext" targetNamespace="urn:t">'
        . $body . '</definitions>';
}

$dir = __DIR__ . '/wsdl_errors_no_bailout';
@mkdir($dir);
$helpers = [
    'not-a-schema.xml' => '<?xml version="1.0"?><root/>',
    'other-tns.xsd' => '<?xml version="1.0"?><xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema" targetNamespace="urn:other"/>',
    'broken-schema.xsd' => '<?xml version="1.0"?><xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema" targetNamespace="urn:b"><xsd:complexType/></xsd:schema>',
    'broken-schema-notns.xsd' => '<?xml version="1.0"?><xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema"><xsd:complexType/></xsd:schema>',
    'broken-import.wsdl' => '<?xml version="1.0"?><definitions xmlns="http://schemas.xmlsoap.org/wsdl/" targetNamespace="urn:t"><message/></definitions>',
];
foreach ($helpers as $name => $content) {
    file_put_contents("$dir/$name", $content);
}

$cases = [
    'sdl_required_ext_defs' => ['extra' => '<x:ext wsdl:required="true"/>'],
    'sdl_required_ext_port' => ['port_body' => '<x:ext wsdl:required="true"/><soap:address location="http://127.0.0.1:1/x"/>'],
    'sdl_unexpected_in_types' => ['types' => '<types><wsdl:foo/></types>'],
    'sdl_import_broken' => ['extra' => '<import namespace="urn:t" location="broken-import.wsdl"/>'],
    'sdl_import_schema_broken' => ['extra' => '<import namespace="urn:t" location="broken-schema.xsd"/>'],
    'sdl_message_dup' => ['messages' => '<message name="Req"><part name="a" type="tns:T"/></message><message name="Resp"><part name="r" type="xsd:string"/></message><message name="Req"/>'],
    'sdl_no_service' => ['service' => ''],
    'sdl_port_no_location' => ['port_body' => '<soap:address/>'],
    'sdl_no_usable_binding' => ['binding' => '<binding name="B" type="tns:PT"><soap:binding style="rpc" transport="urn:not-http"/></binding>'],
    'sdl_operation_not_in_porttype' => ['b_extra' => '<operation name="nope"/>'],
    'sdl_output_unknown_message' => ['port_type' => '<portType name="PT"><operation name="op"><input message="tns:Req"/><output message="tns:Nope"/></operation></portType>'],
    'sdl_part_no_name_second' => ['messages' => '<message name="Req"><part name="a" type="xsd:string"/><part type="xsd:string"/></message><message name="Resp"/>'],
    'sdl_body_missing_part' => ['in_body' => '<soap:body use="encoded" parts="a nope" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>'],
    'sdl_body_unknown_encoding' => ['in_body' => '<soap:body use="encoded" namespace="urn:t" encodingStyle="urn:nope"/>'],
    'sdl_header_unknown_encoding' => ['in_body' => '<soap:body use="encoded" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/><soap:header message="tns:Req" part="a" use="encoded" encodingStyle="urn:nope"/>'],
    'sdl_headerfault_broken' => ['in_body' => '<soap:body use="encoded" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/><soap:header message="tns:Req" part="a" use="literal"><soap:headerfault message="tns:Nope" part="a" use="literal"/></soap:header>'],
    'sdl_header_required_ext' => ['in_body' => '<soap:body use="encoded" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/><soap:header message="tns:Req" part="a" use="literal"><x:ext wsdl:required="true"/></soap:header>'],
    'sdl_fault_two_parts' => ['messages' => '<message name="Req"><part name="a" type="tns:T"/></message><message name="Resp"><part name="r" type="xsd:string"/></message><message name="F"><part name="a" type="xsd:string"/><part name="b" type="xsd:string"/></message>', 'pt_extra' => '<fault name="f" message="tns:F"/>'],
    'sdl_fault_dup' => ['pt_extra' => '<fault name="f" message="tns:Resp"/><fault name="f" message="tns:Resp"/>'],
    'sdl_fault_unknown_encoding' => ['pt_extra' => '<fault name="f" message="tns:Resp"/>', 'b_fault' => '<fault name="f"><soap:fault use="encoded" encodingStyle="urn:nope"/></fault>'],
    'sdl_fault_required_ext' => ['pt_extra' => '<fault name="f" message="tns:Resp"/>', 'b_fault' => '<fault name="f"><x:ext wsdl:required="true"/></fault>'],
    'sdl_second_operation_broken' => ['port_type' => '<portType name="PT"><operation name="op"><input message="tns:Req"/><output message="tns:Resp"/></operation><operation name="op2"><input message="tns:Nope"/></operation></portType>', 'b_extra' => '<operation name="op2"><input><soap:body use="encoded" namespace="urn:t" encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/></input></operation>'],
    'sdl_second_port_broken' => ['svc_extra' => '<port name="P2" binding="tns:Nope"><soap:address location="http://127.0.0.1:1/y"/></port>'],
    'xsd_minoccurs_overflow' => ['schema' => '<xsd:complexType name="T"><xsd:sequence><xsd:element name="a" type="xsd:string" minOccurs="99999999999999999999"/></xsd:sequence></xsd:complexType>'],
    'xsd_any_minoccurs' => ['schema' => '<xsd:complexType name="T"><xsd:sequence><xsd:any minOccurs="-5"/></xsd:sequence></xsd:complexType>'],
    'xsd_restriction_int_overflow' => ['schema' => '<xsd:simpleType name="S"><xsd:restriction base="xsd:int"><xsd:maxInclusive value="99999999999999999999"/></xsd:restriction></xsd:simpleType>'],
    'xsd_enumeration_no_value' => ['schema' => '<xsd:simpleType name="S"><xsd:restriction base="xsd:string"><xsd:enumeration value="a"/><xsd:enumeration/></xsd:restriction></xsd:simpleType>'],
    'xsd_import_no_schema' => ['schema' => '<xsd:import namespace="urn:i" schemaLocation="not-a-schema.xml"/>'],
    'xsd_import_wrong_tns' => ['schema' => '<xsd:import namespace="urn:i" schemaLocation="other-tns.xsd"/>'],
    'xsd_include_wrong_tns' => ['schema' => '<xsd:include schemaLocation="other-tns.xsd"/>'],
    'xsd_import_broken_content' => ['schema' => '<xsd:import namespace="urn:b" schemaLocation="broken-schema.xsd"/>'],
    'xsd_list_subtype_broken' => ['schema' => '<xsd:simpleType name="S"><xsd:list><xsd:simpleType><xsd:foo/></xsd:simpleType></xsd:list></xsd:simpleType>'],
    'xsd_union_subtype_broken' => ['schema' => '<xsd:simpleType name="S"><xsd:union><xsd:simpleType><xsd:foo/></xsd:simpleType></xsd:union></xsd:simpleType>'],
    'xsd_complex_extension_choice_broken' => ['schema' => '<xsd:complexType name="T"><xsd:complexContent><xsd:extension base="tns:T2"><xsd:choice><xsd:foo/></xsd:choice></xsd:extension></xsd:complexContent></xsd:complexType>'],
    'xsd_group_dup' => ['schema' => '<xsd:group name="G"><xsd:sequence/></xsd:group><xsd:group name="G"><xsd:sequence/></xsd:group>'],
    'xsd_element_dup' => ['schema' => '<xsd:element name="E" type="xsd:string"/><xsd:element name="E" type="xsd:int"/>'],
    'xsd_element_anon_complex_broken' => ['schema' => '<xsd:element name="E"><xsd:complexType><xsd:sequence><xsd:foo/></xsd:sequence></xsd:complexType></xsd:element>'],
    'xsd_attribute_dup_in_type' => ['schema' => '<xsd:complexType name="T"><xsd:attribute name="a" type="xsd:string"/><xsd:attribute name="a" type="xsd:int"/></xsd:complexType>'],
    'xsd_attribute_subtype_broken' => ['schema' => '<xsd:complexType name="T"><xsd:attribute name="a"><xsd:simpleType><xsd:foo/></xsd:simpleType></xsd:attribute></xsd:complexType>'],
    'xsd_attributegroup_dup' => ['schema' => '<xsd:attributeGroup name="AG"/><xsd:attributeGroup name="AG"/>'],
    'xsd_attributegroup_attr_broken' => ['schema' => '<xsd:attributeGroup name="AG"><xsd:attribute/></xsd:attributeGroup>'],
    'xsd_recursive_attributegroup' => ['schema' => '<xsd:attributeGroup name="AG"><xsd:attributeGroup ref="tns:AG"/></xsd:attributeGroup><xsd:complexType name="T"><xsd:attributeGroup ref="tns:AG"/></xsd:complexType>'],
    'xsd_recursive_group' => ['schema' => '<xsd:group name="G"><xsd:sequence><xsd:group ref="tns:G"/></xsd:sequence></xsd:group>'],
    'xsd_unresolved_group' => ['schema' => '<xsd:complexType name="T"><xsd:sequence><xsd:group ref="tns:Nope"/></xsd:sequence></xsd:complexType>'],
    'xsd_unresolved_element_deep' => ['schema' => '<xsd:element name="E" type="xsd:string"/><xsd:complexType name="T"><xsd:sequence><xsd:element name="x"><xsd:complexType><xsd:choice><xsd:element ref="tns:Nope"/></xsd:choice></xsd:complexType></xsd:element></xsd:sequence></xsd:complexType>'],
    'xsd_p_choice_any' => ['schema' => '<xsd:complexType name="T"><xsd:choice><xsd:any minOccurs="-1"/></xsd:choice></xsd:complexType>'],
    'xsd_p_recursive_attributegroup_pair' => ['schema' => '<xsd:attributeGroup name="AG1"><xsd:attributeGroup ref="tns:AG2"/></xsd:attributeGroup><xsd:attributeGroup name="AG2"><xsd:attributeGroup ref="tns:AG1"/></xsd:attributeGroup>'],
    'xsd_p_group_ref_later_broken_group' => ['schema' => '<xsd:element name="E" type="xsd:string"/><xsd:group name="G1"><xsd:sequence><xsd:group ref="tns:G2"/></xsd:sequence></xsd:group><xsd:group name="G2"><xsd:sequence><xsd:element ref="tns:Nope"/></xsd:sequence></xsd:group>'],
];
foreach ($cases as $name => $parts) {
    $file = "$dir/$name.wsdl";
    file_put_contents($file, make_wsdl($parts));
    try {
        new SoapClient($file);
        echo "$name: no error\n";
    } catch (SoapFault $e) {
        echo "$name: ", str_replace([$dir, strtr($dir, '\\', '/')], 'DIR', $e->getMessage()), "\n";
    }
    unlink($file);
}
var_dump(gc_status()['protected']);
?>
--CLEAN--
<?php
$dir = __DIR__ . '/wsdl_errors_no_bailout';
foreach (glob("$dir/*") as $file) {
    unlink($file);
}
@rmdir($dir);
?>
--EXPECT--
sdl_required_ext_defs: SOAP-ERROR: Parsing WSDL: Unknown required WSDL extension 'urn:ext'
sdl_required_ext_port: SOAP-ERROR: Parsing WSDL: Unknown required WSDL extension 'urn:ext'
sdl_unexpected_in_types: SOAP-ERROR: Parsing WSDL: Unexpected WSDL element <foo>
sdl_import_broken: SOAP-ERROR: Parsing WSDL: <message> has no name attribute
sdl_import_schema_broken: SOAP-ERROR: Parsing Schema: complexType has no 'name' attribute
sdl_message_dup: SOAP-ERROR: Parsing WSDL: <message> 'Req' already defined
sdl_no_service: SOAP-ERROR: Parsing WSDL: Couldn't bind to service
sdl_port_no_location: SOAP-ERROR: Parsing WSDL: No location associated with <port>
sdl_no_usable_binding: SOAP-ERROR: Parsing WSDL: Could not find any usable binding services in WSDL.
sdl_operation_not_in_porttype: SOAP-ERROR: Parsing WSDL: Missing <portType>/<operation> with name 'nope'
sdl_output_unknown_message: SOAP-ERROR: Parsing WSDL: Missing <message> with name 'tns:Nope'
sdl_part_no_name_second: SOAP-ERROR: Parsing WSDL: No name associated with <part> 'message'
sdl_body_missing_part: SOAP-ERROR: Parsing WSDL: Missing part 'nope' in <message>
sdl_body_unknown_encoding: SOAP-ERROR: Parsing WSDL: Unknown encodingStyle 'urn:nope'
sdl_header_unknown_encoding: SOAP-ERROR: Parsing WSDL: Unknown encodingStyle 'urn:nope'
sdl_headerfault_broken: SOAP-ERROR: Parsing WSDL: Missing <message> with name 'tns:Nope'
sdl_header_required_ext: SOAP-ERROR: Parsing WSDL: Unknown required WSDL extension 'urn:ext'
sdl_fault_two_parts: SOAP-ERROR: Parsing WSDL: The fault message 'tns:F' must have a single part
sdl_fault_dup: SOAP-ERROR: Parsing WSDL: <fault> with name 'f' already defined in 'op'
sdl_fault_unknown_encoding: SOAP-ERROR: Parsing WSDL: Unknown encodingStyle 'urn:nope'
sdl_fault_required_ext: SOAP-ERROR: Parsing WSDL: Unknown required WSDL extension 'urn:ext'
sdl_second_operation_broken: SOAP-ERROR: Parsing WSDL: Missing <message> with name 'tns:Nope'
sdl_second_port_broken: SOAP-ERROR: Parsing WSDL: No <binding> element with name 'Nope'
xsd_minoccurs_overflow: SOAP-ERROR: Parsing Schema: minOccurs value is out of range
xsd_any_minoccurs: SOAP-ERROR: Parsing Schema: minOccurs value is out of range
xsd_restriction_int_overflow: SOAP-ERROR: Parsing Schema: maxInclusive value is out of range
xsd_enumeration_no_value: SOAP-ERROR: Parsing Schema: missing restriction value
xsd_import_no_schema: SOAP-ERROR: Parsing Schema: can't import schema from 'DIR/not-a-schema.xml'
xsd_import_wrong_tns: SOAP-ERROR: Parsing Schema: can't import schema from 'DIR/other-tns.xsd', unexpected 'targetNamespace'='urn:other', expected 'urn:i'
xsd_include_wrong_tns: SOAP-ERROR: Parsing Schema: can't include schema from 'DIR/other-tns.xsd', different 'targetNamespace'
xsd_import_broken_content: SOAP-ERROR: Parsing Schema: complexType has no 'name' attribute
xsd_list_subtype_broken: SOAP-ERROR: Parsing Schema: unexpected <foo> in simpleType
xsd_union_subtype_broken: SOAP-ERROR: Parsing Schema: unexpected <foo> in simpleType
xsd_complex_extension_choice_broken: SOAP-ERROR: Parsing Schema: unexpected <foo> in choice
xsd_group_dup: SOAP-ERROR: Parsing Schema: group 'urn:t:G' already defined
xsd_element_dup: SOAP-ERROR: Parsing Schema: element 'urn:t:E' already defined
xsd_element_anon_complex_broken: SOAP-ERROR: Parsing Schema: unexpected <foo> in sequence
xsd_attribute_dup_in_type: SOAP-ERROR: Parsing Schema: attribute 'urn:t:a' already defined
xsd_attribute_subtype_broken: SOAP-ERROR: Parsing Schema: unexpected <foo> in simpleType
xsd_attributegroup_dup: SOAP-ERROR: Parsing Schema: attributeGroup 'urn:t:AG' already defined
xsd_attributegroup_attr_broken: SOAP-ERROR: Parsing Schema: attribute has no 'name' nor 'ref' attributes
xsd_recursive_attributegroup: SOAP-ERROR: Parsing Schema: recursive attributeGroup 'ref' attribute 'urn:t:AG'
xsd_recursive_group: SOAP-ERROR: Parsing Schema: recursive group 'ref' attribute 'urn:t:G'
xsd_unresolved_group: SOAP-ERROR: Parsing Schema: unresolved group 'ref' attribute 'urn:t:Nope'
xsd_unresolved_element_deep: SOAP-ERROR: Parsing Schema: unresolved element 'ref' attribute 'urn:t:Nope'
xsd_p_choice_any: SOAP-ERROR: Parsing Schema: minOccurs value is out of range
xsd_p_recursive_attributegroup_pair: SOAP-ERROR: Parsing Schema: recursive attributeGroup 'ref' attribute 'urn:t:AG1'
xsd_p_group_ref_later_broken_group: SOAP-ERROR: Parsing Schema: unresolved element 'ref' attribute 'urn:t:Nope'
bool(false)
