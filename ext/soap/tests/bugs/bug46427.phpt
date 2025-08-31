--TEST--
Bug #46427 (SoapClient() stumbles over its "stream_context" parameter)
--EXTENSIONS--
soap
--FILE--
<?php

function getSoapClient_1() {
    $ctx = stream_context_create();
    return new SoapClient(NULL, array(
        'stream_context' => $ctx,
        'location' => 'test://',
        'uri' => 'test://',
        'exceptions' => false));
}

getSoapClient_1()->__soapCall('Help', array());
echo "ok\n";
?>
--EXPECT--
ok
