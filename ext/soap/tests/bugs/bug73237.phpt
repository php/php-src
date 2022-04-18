--TEST--
Bug #73237 "Any" data missing when result includes a struct
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class LocalSoapClient extends SoapClient {
  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    return <<<EOF
<?xml version="1.0" encoding="UTF-8"?><soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns="urn:test.example.org" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:sf="urn:object.test.example.org"><soapenv:Body><queryResponse><result xsi:type="QueryResult"><done>true</done><queryLocator xsi:nil="true"/><records xsi:type="sf:genericObject"><sf:type>CampaignMember</sf:type><sf:Id>00vi0000011VMgeAAG</sf:Id><sf:Id>00vi0000011VMgeAAG</sf:Id><sf:CampaignId>701i0000001lreeAAA</sf:CampaignId><sf:Status>Sent</sf:Status><sf:ContactId xsi:nil="true"/><sf:LeadId>00Qi000001UrbYFEAZ</sf:LeadId><sf:Contact xsi:nil="true"/><sf:Lead xsi:type="sf:genericObject"><sf:type>Lead</sf:type><sf:Id xsi:nil="true"/><sf:Email>angela.lansbury@cbs.com</sf:Email></sf:Lead></records><size>1</size></result></queryResponse></soapenv:Body></soapenv:Envelope>
EOF;
  }
}
$client = new LocalSoapClient(__DIR__."/bug73237.wsdl");
var_dump($client->query(""));
?>
--EXPECT--
object(stdClass)#2 (1) {
  ["result"]=>
  object(stdClass)#3 (4) {
    ["done"]=>
    string(4) "true"
    ["queryLocator"]=>
    NULL
    ["records"]=>
    object(SoapVar)#6 (6) {
      ["enc_type"]=>
      int(0)
      ["enc_value"]=>
      object(stdClass)#4 (3) {
        ["type"]=>
        string(14) "CampaignMember"
        ["Id"]=>
        array(2) {
          [0]=>
          string(18) "00vi0000011VMgeAAG"
          [1]=>
          string(18) "00vi0000011VMgeAAG"
        }
        ["any"]=>
        array(2) {
          [0]=>
          string(175) "<sf:CampaignId>701i0000001lreeAAA</sf:CampaignId><sf:Status>Sent</sf:Status><sf:ContactId xsi:nil="true"/><sf:LeadId>00Qi000001UrbYFEAZ</sf:LeadId><sf:Contact xsi:nil="true"/>"
          ["Lead"]=>
          object(stdClass)#5 (3) {
            ["type"]=>
            string(4) "Lead"
            ["Id"]=>
            NULL
            ["any"]=>
            string(44) "<sf:Email>angela.lansbury@cbs.com</sf:Email>"
          }
        }
      }
      ["enc_stype"]=>
      string(13) "genericObject"
      ["enc_ns"]=>
      string(27) "urn:object.test.example.org"
      ["enc_name"]=>
      NULL
      ["enc_namens"]=>
      NULL
    }
    ["size"]=>
    string(1) "1"
  }
}
