--TEST--
GH-16429 (Segmentation fault (access null pointer) in SoapClient)
--EXTENSIONS--
soap
--FILE--
<?php
function gen() {
    var_dump(str_repeat("x", yield));
}
$gen = gen();
$gen->send(10);
$fusion = $gen;
$client = new SoapClient(__DIR__."/../interop/Round2/GroupB/round2_groupB.wsdl",array("trace"=>1,"exceptions"=>0));
try {
    $client->echo2DStringArray($fusion);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(10) "xxxxxxxxxx"
Cannot traverse an already closed generator
