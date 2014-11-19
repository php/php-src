--TEST--
Default precision is sufficient to serialize all the information in floats
--SKIPIF--
<?php
if (pack('s', 1) != "\x01\x00")
	die("skip test for little-endian architectures");
--FILE--
<?php

$numbers = array(
	"0000000000000000", //0
	"2d431cebe2362a3f", //.0002
	"2e431cebe2362a3f", //.0002 + 10^-Accuracy[.0002]*1.01
	"0000000000001000", //2^-1022. (minimum normal double)
	"0100000000001000", //2^-1022. + 10^-Accuracy[2^-1022.]*1.01
	"ffffffffffffef7f", //2^1024. (maximum normal double)
	"feffffffffffef7f", //2^1024. - 10^-Accuracy[2^1024.]
	"0100000000000000", //minimum subnormal double
	"0200000000000000", //2nd minimum subnormal double
	"fffffffffffff000", //maximum subnormal double
	"fefffffffffff000", //2nd maximum subnormal double
	"0000000000000f7f", //+inf
	"0000000000000fff", //-inf
);

foreach ($numbers as $ns) {
	$num = unpack("d", pack("H*", $ns)); $num = reset($num);
	echo "number: ", sprintf("%.17e", $num), "... ";
	$num2 = unserialize(serialize($num));
	$repr = unpack("H*", pack("d", $num2)); $repr = reset($repr);
	if ($repr == $ns)
		echo "OK\n";
	else
		echo "mismatch\n\twas:    $ns\n\tbecame: $repr\n";
}
--EXPECT--
number: 0.00000000000000000e+0... OK
number: 2.00000000000000010e-4... OK
number: 2.00000000000000037e-4... OK
number: 2.22507385850720138e-308... OK
number: 2.22507385850720188e-308... OK
number: 1.79769313486231571e+308... OK
number: 1.79769313486231551e+308... OK
number: 4.94065645841246544e-324... OK
number: 9.88131291682493088e-324... OK
number: 3.87340857288933536e-304... OK
number: 3.87340857288933455e-304... OK
number: 1.06293653832877718e+304... OK
number: -1.06293653832877718e+304... OK
