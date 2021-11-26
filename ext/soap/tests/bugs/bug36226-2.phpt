--TEST--
Bug #36226 (Inconsistent handling when passing nillable arrays)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$timestamp = "2005-11-08T11:22:07+03:00";
$wsdl = __DIR__."/bug36226-2.wsdl";

function PostEvents($x) {
  var_dump($x);
  exit();
  return $x;
}

class TestSoapClient extends SoapClient {
  private $server;

  function __construct($wsdl, $options) {
    parent::__construct($wsdl, $options);
    $this->server = new SoapServer($wsdl, $options);
    $this->server->addFunction('PostEvents');
  }

  function __doRequest($request, $location, $action, $version, $one_way = 0): ?string {
    echo "$request\n";
    $this->server->handle($request);
    return $response;
  }
}

$soapClient = new TestSoapClient($wsdl,
  array(
    'trace' => 1,
    'exceptions' => 0,
    'classmap' => array(
      'logOnEvent' => 'LogOnEvent',
      'logOffEvent' => 'LogOffEvent',
      'events' => 'IVREvents'
    ),
    'features' => SOAP_SINGLE_ELEMENT_ARRAYS
  ));

$logOnEvent = null;
//$logOnEvent = array();
$logOffEvents[] = new LogOffEvent(34567, $timestamp, "Smoked");
//$logOffEvents[] = new LogOffEvent(34568, $timestamp, "SmokeFree");
$ivrEvents = new IVREvents("1.0", 101, 12345, 'IVR', $logOnEvent, $logOffEvents);
$result = $soapClient->PostEvents($ivrEvents);

class LogOffEvent {
  public $audienceMemberId;
  public $timestamp;
  public $smokeStatus;
  public $callInitiator;

  function __construct($audienceMemberId, $timestamp, $smokeStatus) {
    $this->audienceMemberId = $audienceMemberId;
    $this->timestamp = $timestamp;
    $this->smokeStatus = $smokeStatus;
    $this->callInitiator = "IVR";
  }
}

class LogOnEvent {
  public $audienceMemberId;
  public $timestamp;

  function __construct($audienceMemberId, $timestamp) {
    $this->audienceMemberId = $audienceMemberId;
    $this->timestamp = $timestamp;
  }
}

class IVREvents {
  public $version;
  public $activityId;
  public $messageId;
  public $source;
  public $logOnEvent;
  public $logOffEvent;

  function __construct($version, $activityId, $messageId, $source, $logOnEvent=NULL, $logOffEvent=NULL) {
    $this->version = $version;
    $this->activityId = $activityId;
    $this->messageId = $messageId;
    $this->source = $source;
    $this->logOnEvent = $logOnEvent;
    $this->logOffEvent = $logOffEvent;
  }

}
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testurl/Message" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><SOAP-ENV:Body><ns1:ivrEvents version="1.0" activityId="101" messageId="12345" source="IVR"><ns1:logOffEvent audienceMemberId="34567" timestamp="2005-11-08T11:22:07+03:00" smokeStatus="Smoked" callInitiator="IVR"/><ns1:logOnEvent xsi:nil="true"/></ns1:ivrEvents></SOAP-ENV:Body></SOAP-ENV:Envelope>

object(IVREvents)#5 (6) {
  ["version"]=>
  string(3) "1.0"
  ["activityId"]=>
  int(101)
  ["messageId"]=>
  int(12345)
  ["source"]=>
  string(3) "IVR"
  ["logOnEvent"]=>
  NULL
  ["logOffEvent"]=>
  array(1) {
    [0]=>
    object(LogOffEvent)#6 (4) {
      ["audienceMemberId"]=>
      int(34567)
      ["timestamp"]=>
      string(25) "2005-11-08T11:22:07+03:00"
      ["smokeStatus"]=>
      string(6) "Smoked"
      ["callInitiator"]=>
      string(3) "IVR"
    }
  }
}
