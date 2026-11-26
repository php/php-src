--TEST--
SOAP typemap 14: to_xml() output with a DOCTYPE or processing instruction before the root
--EXTENSIONS--
soap
--FILE--
<?php
$request = <<<XML
<env:Envelope xmlns:env="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:ns1="http://schemas.nothing.com">
  <env:Body>
    <ns1:dotest/>
  </env:Body>
</env:Envelope>
XML;

function book_to_xml($book) {
    return $GLOBALS['prolog'] . '<book><a>' . $book->a . '</a></book>';
}

class book {
    public $a = "foo";
}

function dotest() {
    return new SoapVar(new book, null, "book", "http://schemas.nothing.com");
}

$prologs = [
    '<!DOCTYPE book>',
    '<!DOCTYPE book [<!NOTATION n SYSTEM "x"><!ATTLIST book id ID #IMPLIED>]>',
    '<?pi data?>',
];

foreach ($prologs as $prolog) {
    $GLOBALS['prolog'] = $prolog;
    $server = new SoapServer(null, [
        'uri'     => "http://schemas.nothing.com",
        'typemap' => [[
            "type_ns"   => "http://schemas.nothing.com",
            "type_name" => "book",
            "to_xml"    => "book_to_xml",
        ]],
    ]);
    $server->addFunction("dotest");
    $server->handle($request);
}
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotestResponse><book xsi:type="ns1:book"><a>foo</a></book></ns1:dotestResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotestResponse><book xsi:type="ns1:book"><a>foo</a></book></ns1:dotestResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://schemas.nothing.com" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:dotestResponse><book xsi:type="ns1:book"><a>foo</a></book></ns1:dotestResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
