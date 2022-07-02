--TEST--
Bug #44882 (SOAP extension object decoding bug)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class TestSoapClient extends SoapClient
{
    public function __doRequest($req, $loc, $act, $ver, $one_way = 0): ?string
    {
        return <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope
  xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns:ns="urn:ebay:api:PayPalAPI">
  <SOAP-ENV:Body id="_0">
    <GetExpressCheckoutDetailsResponse xmlns="urn:ebay:api:PayPalAPI">
      <Timestamp>2008-06-23T14:51:08Z</Timestamp>
      <Ack>Success</Ack>
      <CorrelationID>ae013a0ccdf13</CorrelationID>
      <Version>50.000000</Version>
      <Build>588340</Build>
      <GetExpressCheckoutDetailsResponseDetails xsi:type="ns:GetExpressCheckoutDetailsResponseDetailsType">
        <Token>EC-11Y75137T2399952C</Token>
        <PayerInfo>
          <Payer>example@example.com</Payer>
          <PayerID>MU82WA43YXM9C</PayerID>
          <PayerStatus>verified</PayerStatus>
        </PayerInfo>
      </GetExpressCheckoutDetailsResponseDetails>
    </GetExpressCheckoutDetailsResponse>
  </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;
    }
}

$client = new TestSoapClient(__DIR__.'/bug44882.wsdl');
print_r($client->GetExpressCheckoutDetails());
?>
--EXPECT--
stdClass Object
(
    [Timestamp] => 2008-06-23T14:51:08Z
    [Ack] => Success
    [CorrelationID] => ae013a0ccdf13
    [Version] => 50.000000
    [Build] => 588340
    [GetExpressCheckoutDetailsResponseDetails] => stdClass Object
        (
            [Token] => EC-11Y75137T2399952C
            [PayerInfo] => stdClass Object
                (
                    [Payer] => example@example.com
                    [PayerID] => MU82WA43YXM9C
                    [PayerStatus] => verified
                )

        )

)
