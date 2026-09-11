--TEST--
CurlAddressFamily enum
--EXTENSIONS--
curl
--FILE--
<?php
var_dump(CurlAddressFamily::Inet, CurlAddressFamily::Inet6, CurlAddressFamily::Unix);

$re = new ReflectionEnum(CurlAddressFamily::class);
var_dump($re->isEnum(), $re->getBackingType());

foreach (CurlAddressFamily::cases() as $case) {
    echo $case->name, PHP_EOL;
}

var_dump(unserialize(serialize(CurlAddressFamily::Inet)) === CurlAddressFamily::Inet);

echo "Done";
?>
--EXPECT--
enum(CurlAddressFamily::Inet)
enum(CurlAddressFamily::Inet6)
enum(CurlAddressFamily::Unix)
bool(true)
NULL
Inet
Inet6
Unix
bool(true)
Done
