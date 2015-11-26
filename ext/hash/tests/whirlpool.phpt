--TEST--
whirlpool
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php
echo hash('whirlpool', ''), "\n";
echo hash('whirlpool', $s='---qwertzuiopasdfghjklyxcvbnm------qwertzuiopasdfghjklyxcvbnm---'), "\n";
echo hash('whirlpool', str_repeat($s.'0', 1000)), "\n";
?>
--EXPECT--
19fa61d75522a4669b44e39c1d2e1726c530232130d407f89afee0964997f7a73e83be698b288febcf88e3e03c4f0757ea8964e59b63d93708b138cc42a66eb3
916ce6431d2f384be68d96bcaba800c21b82e9cc2f07076554c9557f85476b5d8f2b263951121fa955e34b31a4cdc857bdf076b123c2252543dcef34f84a7ef3
b51984710d11893ac08e10529519f9801d82ea534629d14bc8c810307934496017ccdf23bfcb62c7e1259664e84c9388ff646b0b46688b0a6c32e5571234dd95
