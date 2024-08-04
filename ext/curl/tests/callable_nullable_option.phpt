--TEST--
Callable options are nullable
--EXTENSIONS--
curl
--FILE--
<?php

$ch = curl_init();
curl_setopt_array($ch, [
    CURLOPT_PROGRESSFUNCTION => null,
]);

?>
===DONE===
--EXPECT--
===DONE===
