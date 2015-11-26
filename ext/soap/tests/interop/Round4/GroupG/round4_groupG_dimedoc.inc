<?php
class SOAP_Interop_GroupG {

  function EchoBase64AsAttachment($in) {
    return $in;
  }

  function EchoAttachmentAsBase64($in) {
    return $in;
  }

  function EchoAttachment($in) {
    return $in;
  }

  function EchoAttachments($in) {
    return $in;
  }

  function EchoAttachmentAsString($in) {
    return $in;
  }

  function EchoUnrefAttachments($in) {
    return $in;
  }

}

$server = new SoapServer(dirname(__FILE__)."/round4_groupG_dimedoc.wsdl");
$server->setClass("SOAP_Interop_GroupG");
$server->handle($HTTP_RAW_POST_DATA);
?>
