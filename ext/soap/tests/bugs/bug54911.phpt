--TEST--
Bug #54911 (Access to a undefined member in inherit SoapClient may cause Segmentation Fault)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    class XSoapClient extends SoapClient {
        function __doRequest($request, $location, $action, $version, $one_way=false) {
            echo self::$crash;
        }
    }
    $client = new XSoapClient(null, array('uri'=>'', 'location'=>''));
    $client->__soapCall('', array());
?>
--EXPECTF--
Fatal error: Uncaught SoapFault exception: [Client] Access to undeclared static property XSoapClient::$crash in %sbug54911.php:4
Stack trace:
#0 [internal function]: XSoapClient->__doRequest('<?xml version="...', '', '#', 1, false)
#1 %sbug54911.php(8): SoapClient->__soapCall('', Array)
#2 {main}
  thrown in %sbug54911.php on line 4
