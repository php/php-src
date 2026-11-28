--TEST--
GH-25604: IPv4 range warnings on accepted final parts are non-fatal
--EXTENSIONS--
uri
--FILE--
<?php
foreach (['127.0.0.1', '2130706433', '4294967295', '4294967296', '127.65535.',
          '127.0.65536', '256.1', '2130706433:65536'] as $authority) {
    $url = Uri\WhatWg\Url::parse("http://$authority/", null, $errors);
    echo $authority, ' => ', $url?->getAsciiHost() ?? 'failed', "\n";
    foreach ($errors as $error) {
        echo $error->type->name, ' ', $error->failure ? 'fatal' : 'warning', "\n";
    }
}
?>
--EXPECT--
127.0.0.1 => 127.0.0.1
2130706433 => 127.0.0.1
Ipv4OutOfRangePart warning
4294967295 => 255.255.255.255
Ipv4OutOfRangePart warning
4294967296 => failed
Ipv4OutOfRangePart fatal
127.65535. => 127.0.255.255
Ipv4EmptyPart warning
Ipv4OutOfRangePart warning
127.0.65536 => failed
Ipv4OutOfRangePart fatal
256.1 => failed
Ipv4OutOfRangePart fatal
2130706433:65536 => failed
PortOutOfRange fatal
Ipv4OutOfRangePart warning
