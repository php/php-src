<?php
class SOAP_Interop_GroupD {
    function echoPerson($person)
    {
        return $person;
    }

    function echoDocument($doc)
    {
        return $doc;
    }
}

$server = new SoapServer(dirname(__FILE__)."/round3_groupD_compound1.wsdl");
$server->setClass("SOAP_Interop_GroupD");
$server->handle($HTTP_RAW_POST_DATA);
?>
