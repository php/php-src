--TEST--
Bug GH-8538 (SoapClient may strip parts of nmtokens)
--SKIPIF--
<?php require_once("skipif.inc"); ?>
--FILE--
<?php
$client = new SoapClient(__DIR__ . "/gh8538.wsdl"); 
var_dump($client->__getFunctions());
?>
--EXPECT--
array(9) {
  [0]=>
  string(71) "reportHealthViewResponse reportHealthView(reportHealthView $parameters)"
  [1]=>
  string(59) "reportPolicyResponse reportPolicy(reportPolicy $parameters)"
  [2]=>
  string(56) "queryPolicyResponse queryPolicy(queryPolicy $parameters)"
  [3]=>
  string(59) "reportConfigResponse reportConfig(reportConfig $parameters)"
  [4]=>
  string(56) "reportEventResponse reportEvent(reportEvent $parameters)"
  [5]=>
  string(53) "reportViewResponse reportView(reportView $parameters)"
  [6]=>
  string(56) "queryConfigResponse queryConfig(queryConfig $parameters)"
  [7]=>
  string(50) "queryViewResponse queryView(queryView $parameters)"
  [8]=>
  string(50) "reportLogResponse reportLog(reportLog $parameters)"
}
