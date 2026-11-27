--TEST--
gethostaddresses() basic functionality
--FILE--
<?php
$addresses = gethostaddresses("localhost");
var_dump(is_array($addresses) && array_is_list($addresses) && count($addresses) > 0);
foreach ($addresses as $address) {
    if (filter_var($address, FILTER_VALIDATE_IP) === false) {
        echo "Invalid address: ", $address, PHP_EOL;
    }
}

var_dump(gethostaddresses("127.0.0.1"));
var_dump(gethostaddresses(str_repeat("a", 2000)));

try {
    gethostaddresses("local\0host");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
bool(true)
array(1) {
  [0]=>
  string(9) "127.0.0.1"
}

Warning: gethostaddresses(): Host name cannot be longer than %d characters in %s on line %d
bool(false)
ValueError: gethostaddresses(): Argument #1 ($hostname) must not contain any null bytes
