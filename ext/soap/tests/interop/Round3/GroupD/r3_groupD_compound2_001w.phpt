--TEST--
SOAP Interop Round3 GroupD Compound2 001 (php/wsdl): echoEmployee
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
precision=14
soap.wsdl_cache_enabled=0
--FILE--
<?php
class Person {
    function __construct($a=NULL, $i=NULL, $n=NULL, $m=NULL) {
        $this->Age = $a;
        $this->ID = $i;
        $this->Name = $n;
        $this->Male = $m;
    }
}
class Employee {
    function __construct($person=NULL,$id=NULL,$salary=NULL) {
        $this->person = $person;
        $this->ID = $id;
        $this->salary = $salary;
    }
}
$person = new Person(32,12345,'Shane',TRUE);
$employee = new Employee($person,12345,1000000.00);

$client = new SoapClient(__DIR__."/round3_groupD_compound2.wsdl",array("trace"=>1,"exceptions"=>0));
$client->echoEmployee($employee);
echo $client->__getlastrequest();
$HTTP_RAW_POST_DATA = $client->__getlastrequest();
include("round3_groupD_compound2.inc");
echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/person" xmlns:ns2="http://soapinterop.org/employee"><SOAP-ENV:Body><ns2:x_Employee><ns2:person><ns1:Name>Shane</ns1:Name><ns1:Male>true</ns1:Male></ns2:person><ns2:salary>1000000</ns2:salary><ns2:ID>12345</ns2:ID></ns2:x_Employee></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://soapinterop.org/person" xmlns:ns2="http://soapinterop.org/employee"><SOAP-ENV:Body><ns2:result_Employee><ns2:person><ns1:Name>Shane</ns1:Name><ns1:Male>true</ns1:Male></ns2:person><ns2:salary>1000000</ns2:salary><ns2:ID>12345</ns2:ID></ns2:result_Employee></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
