<?php
include "SOAP/Server.php";

function testMethod($arg) {
    if ($arg != "") {
        return crypt($arg);
    } else {
        return "Please supply a text";
    }    
}

$server = new soap_server();

$server->addToMap("testMethod",array("string"),array("string"));
$server->service($HTTP_RAW_POST_DATA);
echo "<!-- this is a test -->";
?>