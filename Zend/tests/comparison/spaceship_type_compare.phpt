--TEST--
Compare types via <=> operator
--FILE--
<?php

function cmp($l, $r) {
	echo @match ($l <=> $r) {
		0 => "EQUAL",
		1 => match ($r <=> $l) {
			-1 => "GREATER THAN",
			1 => "UNCOMPARABLE",
		},
		-1 => "LESS THAN",
	};
}

class test {
}

$values = [
	'null' => null,
	'false' => false,
	'true' => true,
	'i-5' => -5,
	'i0' => 0,
	'i10' => 10,
	'f-5' => -5.0,
	'f0' => 0.0,
	'f2.5' => 2.5,
	'f10' => 10.0,
	's""' => "",
	's"str"' => "str",
	's"-5"' => "-5",
	's"0"' => "0",
	's"2.5"' => "2.5",
	's"10"' => "10",
	'a[]' => [],
	'a[1, 2, 3]' => [1, 2, 3],
	'c"stdClass"' => new stdClass(),
	'c"test"' => new test(),
	'r"STDOUT"' => STDOUT,
	'r"STDERR"' => STDERR,
];

foreach($values as $l_name => $l) {
	foreach($values as $r_name => $r) {
		echo $l_name, ' ', cmp($l, $r), ' ', $r_name, "\n";
    }
}

?>
--EXPECT--
null EQUAL null
null EQUAL false
null LESS THAN true
null LESS THAN i-5
null EQUAL i0
null LESS THAN i10
null LESS THAN f-5
null EQUAL f0
null LESS THAN f2.5
null LESS THAN f10
null EQUAL s""
null LESS THAN s"str"
null LESS THAN s"-5"
null LESS THAN s"0"
null LESS THAN s"2.5"
null LESS THAN s"10"
null EQUAL a[]
null LESS THAN a[1, 2, 3]
null LESS THAN c"stdClass"
null LESS THAN c"test"
null LESS THAN r"STDOUT"
null LESS THAN r"STDERR"
false EQUAL null
false EQUAL false
false LESS THAN true
false LESS THAN i-5
false EQUAL i0
false LESS THAN i10
false LESS THAN f-5
false EQUAL f0
false LESS THAN f2.5
false LESS THAN f10
false EQUAL s""
false LESS THAN s"str"
false LESS THAN s"-5"
false EQUAL s"0"
false LESS THAN s"2.5"
false LESS THAN s"10"
false EQUAL a[]
false LESS THAN a[1, 2, 3]
false LESS THAN c"stdClass"
false LESS THAN c"test"
false LESS THAN r"STDOUT"
false LESS THAN r"STDERR"
true GREATER THAN null
true GREATER THAN false
true EQUAL true
true EQUAL i-5
true GREATER THAN i0
true EQUAL i10
true EQUAL f-5
true GREATER THAN f0
true EQUAL f2.5
true EQUAL f10
true GREATER THAN s""
true EQUAL s"str"
true EQUAL s"-5"
true GREATER THAN s"0"
true EQUAL s"2.5"
true EQUAL s"10"
true GREATER THAN a[]
true EQUAL a[1, 2, 3]
true EQUAL c"stdClass"
true EQUAL c"test"
true EQUAL r"STDOUT"
true EQUAL r"STDERR"
i-5 GREATER THAN null
i-5 GREATER THAN false
i-5 EQUAL true
i-5 EQUAL i-5
i-5 LESS THAN i0
i-5 LESS THAN i10
i-5 EQUAL f-5
i-5 LESS THAN f0
i-5 LESS THAN f2.5
i-5 LESS THAN f10
i-5 GREATER THAN s""
i-5 LESS THAN s"str"
i-5 EQUAL s"-5"
i-5 LESS THAN s"0"
i-5 LESS THAN s"2.5"
i-5 LESS THAN s"10"
i-5 LESS THAN a[]
i-5 LESS THAN a[1, 2, 3]
i-5 LESS THAN c"stdClass"
i-5 LESS THAN c"test"
i-5 LESS THAN r"STDOUT"
i-5 LESS THAN r"STDERR"
i0 EQUAL null
i0 EQUAL false
i0 LESS THAN true
i0 GREATER THAN i-5
i0 EQUAL i0
i0 LESS THAN i10
i0 GREATER THAN f-5
i0 EQUAL f0
i0 LESS THAN f2.5
i0 LESS THAN f10
i0 GREATER THAN s""
i0 LESS THAN s"str"
i0 GREATER THAN s"-5"
i0 EQUAL s"0"
i0 LESS THAN s"2.5"
i0 LESS THAN s"10"
i0 LESS THAN a[]
i0 LESS THAN a[1, 2, 3]
i0 LESS THAN c"stdClass"
i0 LESS THAN c"test"
i0 LESS THAN r"STDOUT"
i0 LESS THAN r"STDERR"
i10 GREATER THAN null
i10 GREATER THAN false
i10 EQUAL true
i10 GREATER THAN i-5
i10 GREATER THAN i0
i10 EQUAL i10
i10 GREATER THAN f-5
i10 GREATER THAN f0
i10 GREATER THAN f2.5
i10 EQUAL f10
i10 GREATER THAN s""
i10 LESS THAN s"str"
i10 GREATER THAN s"-5"
i10 GREATER THAN s"0"
i10 GREATER THAN s"2.5"
i10 EQUAL s"10"
i10 LESS THAN a[]
i10 LESS THAN a[1, 2, 3]
i10 GREATER THAN c"stdClass"
i10 GREATER THAN c"test"
i10 GREATER THAN r"STDOUT"
i10 GREATER THAN r"STDERR"
f-5 GREATER THAN null
f-5 GREATER THAN false
f-5 EQUAL true
f-5 EQUAL i-5
f-5 LESS THAN i0
f-5 LESS THAN i10
f-5 EQUAL f-5
f-5 LESS THAN f0
f-5 LESS THAN f2.5
f-5 LESS THAN f10
f-5 GREATER THAN s""
f-5 LESS THAN s"str"
f-5 EQUAL s"-5"
f-5 LESS THAN s"0"
f-5 LESS THAN s"2.5"
f-5 LESS THAN s"10"
f-5 LESS THAN a[]
f-5 LESS THAN a[1, 2, 3]
f-5 LESS THAN c"stdClass"
f-5 LESS THAN c"test"
f-5 LESS THAN r"STDOUT"
f-5 LESS THAN r"STDERR"
f0 EQUAL null
f0 EQUAL false
f0 LESS THAN true
f0 GREATER THAN i-5
f0 EQUAL i0
f0 LESS THAN i10
f0 GREATER THAN f-5
f0 EQUAL f0
f0 LESS THAN f2.5
f0 LESS THAN f10
f0 GREATER THAN s""
f0 LESS THAN s"str"
f0 GREATER THAN s"-5"
f0 EQUAL s"0"
f0 LESS THAN s"2.5"
f0 LESS THAN s"10"
f0 LESS THAN a[]
f0 LESS THAN a[1, 2, 3]
f0 LESS THAN c"stdClass"
f0 LESS THAN c"test"
f0 LESS THAN r"STDOUT"
f0 LESS THAN r"STDERR"
f2.5 GREATER THAN null
f2.5 GREATER THAN false
f2.5 EQUAL true
f2.5 GREATER THAN i-5
f2.5 GREATER THAN i0
f2.5 LESS THAN i10
f2.5 GREATER THAN f-5
f2.5 GREATER THAN f0
f2.5 EQUAL f2.5
f2.5 LESS THAN f10
f2.5 GREATER THAN s""
f2.5 LESS THAN s"str"
f2.5 GREATER THAN s"-5"
f2.5 GREATER THAN s"0"
f2.5 EQUAL s"2.5"
f2.5 LESS THAN s"10"
f2.5 LESS THAN a[]
f2.5 LESS THAN a[1, 2, 3]
f2.5 GREATER THAN c"stdClass"
f2.5 GREATER THAN c"test"
f2.5 GREATER THAN r"STDOUT"
f2.5 LESS THAN r"STDERR"
f10 GREATER THAN null
f10 GREATER THAN false
f10 EQUAL true
f10 GREATER THAN i-5
f10 GREATER THAN i0
f10 EQUAL i10
f10 GREATER THAN f-5
f10 GREATER THAN f0
f10 GREATER THAN f2.5
f10 EQUAL f10
f10 GREATER THAN s""
f10 LESS THAN s"str"
f10 GREATER THAN s"-5"
f10 GREATER THAN s"0"
f10 GREATER THAN s"2.5"
f10 EQUAL s"10"
f10 LESS THAN a[]
f10 LESS THAN a[1, 2, 3]
f10 GREATER THAN c"stdClass"
f10 GREATER THAN c"test"
f10 GREATER THAN r"STDOUT"
f10 GREATER THAN r"STDERR"
s"" EQUAL null
s"" EQUAL false
s"" LESS THAN true
s"" LESS THAN i-5
s"" LESS THAN i0
s"" LESS THAN i10
s"" LESS THAN f-5
s"" LESS THAN f0
s"" LESS THAN f2.5
s"" LESS THAN f10
s"" EQUAL s""
s"" LESS THAN s"str"
s"" LESS THAN s"-5"
s"" LESS THAN s"0"
s"" LESS THAN s"2.5"
s"" LESS THAN s"10"
s"" LESS THAN a[]
s"" LESS THAN a[1, 2, 3]
s"" LESS THAN c"stdClass"
s"" LESS THAN c"test"
s"" LESS THAN r"STDOUT"
s"" LESS THAN r"STDERR"
s"str" GREATER THAN null
s"str" GREATER THAN false
s"str" EQUAL true
s"str" GREATER THAN i-5
s"str" GREATER THAN i0
s"str" GREATER THAN i10
s"str" GREATER THAN f-5
s"str" GREATER THAN f0
s"str" GREATER THAN f2.5
s"str" GREATER THAN f10
s"str" GREATER THAN s""
s"str" EQUAL s"str"
s"str" GREATER THAN s"-5"
s"str" GREATER THAN s"0"
s"str" GREATER THAN s"2.5"
s"str" GREATER THAN s"10"
s"str" LESS THAN a[]
s"str" LESS THAN a[1, 2, 3]
s"str" LESS THAN c"stdClass"
s"str" LESS THAN c"test"
s"str" LESS THAN r"STDOUT"
s"str" LESS THAN r"STDERR"
s"-5" GREATER THAN null
s"-5" GREATER THAN false
s"-5" EQUAL true
s"-5" EQUAL i-5
s"-5" LESS THAN i0
s"-5" LESS THAN i10
s"-5" EQUAL f-5
s"-5" LESS THAN f0
s"-5" LESS THAN f2.5
s"-5" LESS THAN f10
s"-5" GREATER THAN s""
s"-5" LESS THAN s"str"
s"-5" EQUAL s"-5"
s"-5" LESS THAN s"0"
s"-5" LESS THAN s"2.5"
s"-5" LESS THAN s"10"
s"-5" LESS THAN a[]
s"-5" LESS THAN a[1, 2, 3]
s"-5" LESS THAN c"stdClass"
s"-5" LESS THAN c"test"
s"-5" LESS THAN r"STDOUT"
s"-5" LESS THAN r"STDERR"
s"0" GREATER THAN null
s"0" EQUAL false
s"0" LESS THAN true
s"0" GREATER THAN i-5
s"0" EQUAL i0
s"0" LESS THAN i10
s"0" GREATER THAN f-5
s"0" EQUAL f0
s"0" LESS THAN f2.5
s"0" LESS THAN f10
s"0" GREATER THAN s""
s"0" LESS THAN s"str"
s"0" GREATER THAN s"-5"
s"0" EQUAL s"0"
s"0" LESS THAN s"2.5"
s"0" LESS THAN s"10"
s"0" LESS THAN a[]
s"0" LESS THAN a[1, 2, 3]
s"0" LESS THAN c"stdClass"
s"0" LESS THAN c"test"
s"0" LESS THAN r"STDOUT"
s"0" LESS THAN r"STDERR"
s"2.5" GREATER THAN null
s"2.5" GREATER THAN false
s"2.5" EQUAL true
s"2.5" GREATER THAN i-5
s"2.5" GREATER THAN i0
s"2.5" LESS THAN i10
s"2.5" GREATER THAN f-5
s"2.5" GREATER THAN f0
s"2.5" EQUAL f2.5
s"2.5" LESS THAN f10
s"2.5" GREATER THAN s""
s"2.5" LESS THAN s"str"
s"2.5" GREATER THAN s"-5"
s"2.5" GREATER THAN s"0"
s"2.5" EQUAL s"2.5"
s"2.5" LESS THAN s"10"
s"2.5" LESS THAN a[]
s"2.5" LESS THAN a[1, 2, 3]
s"2.5" LESS THAN c"stdClass"
s"2.5" LESS THAN c"test"
s"2.5" GREATER THAN r"STDOUT"
s"2.5" LESS THAN r"STDERR"
s"10" GREATER THAN null
s"10" GREATER THAN false
s"10" EQUAL true
s"10" GREATER THAN i-5
s"10" GREATER THAN i0
s"10" EQUAL i10
s"10" GREATER THAN f-5
s"10" GREATER THAN f0
s"10" GREATER THAN f2.5
s"10" EQUAL f10
s"10" GREATER THAN s""
s"10" LESS THAN s"str"
s"10" GREATER THAN s"-5"
s"10" GREATER THAN s"0"
s"10" GREATER THAN s"2.5"
s"10" EQUAL s"10"
s"10" LESS THAN a[]
s"10" LESS THAN a[1, 2, 3]
s"10" LESS THAN c"stdClass"
s"10" LESS THAN c"test"
s"10" GREATER THAN r"STDOUT"
s"10" GREATER THAN r"STDERR"
a[] EQUAL null
a[] EQUAL false
a[] LESS THAN true
a[] GREATER THAN i-5
a[] GREATER THAN i0
a[] GREATER THAN i10
a[] GREATER THAN f-5
a[] GREATER THAN f0
a[] GREATER THAN f2.5
a[] GREATER THAN f10
a[] GREATER THAN s""
a[] GREATER THAN s"str"
a[] GREATER THAN s"-5"
a[] GREATER THAN s"0"
a[] GREATER THAN s"2.5"
a[] GREATER THAN s"10"
a[] EQUAL a[]
a[] LESS THAN a[1, 2, 3]
a[] LESS THAN c"stdClass"
a[] LESS THAN c"test"
a[] GREATER THAN r"STDOUT"
a[] GREATER THAN r"STDERR"
a[1, 2, 3] GREATER THAN null
a[1, 2, 3] GREATER THAN false
a[1, 2, 3] EQUAL true
a[1, 2, 3] GREATER THAN i-5
a[1, 2, 3] GREATER THAN i0
a[1, 2, 3] GREATER THAN i10
a[1, 2, 3] GREATER THAN f-5
a[1, 2, 3] GREATER THAN f0
a[1, 2, 3] GREATER THAN f2.5
a[1, 2, 3] GREATER THAN f10
a[1, 2, 3] GREATER THAN s""
a[1, 2, 3] GREATER THAN s"str"
a[1, 2, 3] GREATER THAN s"-5"
a[1, 2, 3] GREATER THAN s"0"
a[1, 2, 3] GREATER THAN s"2.5"
a[1, 2, 3] GREATER THAN s"10"
a[1, 2, 3] GREATER THAN a[]
a[1, 2, 3] EQUAL a[1, 2, 3]
a[1, 2, 3] LESS THAN c"stdClass"
a[1, 2, 3] LESS THAN c"test"
a[1, 2, 3] GREATER THAN r"STDOUT"
a[1, 2, 3] GREATER THAN r"STDERR"
c"stdClass" GREATER THAN null
c"stdClass" GREATER THAN false
c"stdClass" EQUAL true
c"stdClass" GREATER THAN i-5
c"stdClass" GREATER THAN i0
c"stdClass" LESS THAN i10
c"stdClass" GREATER THAN f-5
c"stdClass" GREATER THAN f0
c"stdClass" LESS THAN f2.5
c"stdClass" LESS THAN f10
c"stdClass" GREATER THAN s""
c"stdClass" GREATER THAN s"str"
c"stdClass" GREATER THAN s"-5"
c"stdClass" GREATER THAN s"0"
c"stdClass" GREATER THAN s"2.5"
c"stdClass" GREATER THAN s"10"
c"stdClass" GREATER THAN a[]
c"stdClass" GREATER THAN a[1, 2, 3]
c"stdClass" EQUAL c"stdClass"
c"stdClass" UNCOMPARABLE c"test"
c"stdClass" GREATER THAN r"STDOUT"
c"stdClass" GREATER THAN r"STDERR"
c"test" GREATER THAN null
c"test" GREATER THAN false
c"test" EQUAL true
c"test" GREATER THAN i-5
c"test" GREATER THAN i0
c"test" LESS THAN i10
c"test" GREATER THAN f-5
c"test" GREATER THAN f0
c"test" LESS THAN f2.5
c"test" LESS THAN f10
c"test" GREATER THAN s""
c"test" GREATER THAN s"str"
c"test" GREATER THAN s"-5"
c"test" GREATER THAN s"0"
c"test" GREATER THAN s"2.5"
c"test" GREATER THAN s"10"
c"test" GREATER THAN a[]
c"test" GREATER THAN a[1, 2, 3]
c"test" UNCOMPARABLE c"stdClass"
c"test" EQUAL c"test"
c"test" GREATER THAN r"STDOUT"
c"test" GREATER THAN r"STDERR"
r"STDOUT" GREATER THAN null
r"STDOUT" GREATER THAN false
r"STDOUT" EQUAL true
r"STDOUT" GREATER THAN i-5
r"STDOUT" GREATER THAN i0
r"STDOUT" LESS THAN i10
r"STDOUT" GREATER THAN f-5
r"STDOUT" GREATER THAN f0
r"STDOUT" LESS THAN f2.5
r"STDOUT" LESS THAN f10
r"STDOUT" GREATER THAN s""
r"STDOUT" GREATER THAN s"str"
r"STDOUT" GREATER THAN s"-5"
r"STDOUT" GREATER THAN s"0"
r"STDOUT" LESS THAN s"2.5"
r"STDOUT" LESS THAN s"10"
r"STDOUT" LESS THAN a[]
r"STDOUT" LESS THAN a[1, 2, 3]
r"STDOUT" LESS THAN c"stdClass"
r"STDOUT" LESS THAN c"test"
r"STDOUT" EQUAL r"STDOUT"
r"STDOUT" LESS THAN r"STDERR"
r"STDERR" GREATER THAN null
r"STDERR" GREATER THAN false
r"STDERR" EQUAL true
r"STDERR" GREATER THAN i-5
r"STDERR" GREATER THAN i0
r"STDERR" LESS THAN i10
r"STDERR" GREATER THAN f-5
r"STDERR" GREATER THAN f0
r"STDERR" GREATER THAN f2.5
r"STDERR" LESS THAN f10
r"STDERR" GREATER THAN s""
r"STDERR" GREATER THAN s"str"
r"STDERR" GREATER THAN s"-5"
r"STDERR" GREATER THAN s"0"
r"STDERR" GREATER THAN s"2.5"
r"STDERR" LESS THAN s"10"
r"STDERR" LESS THAN a[]
r"STDERR" LESS THAN a[1, 2, 3]
r"STDERR" LESS THAN c"stdClass"
r"STDERR" LESS THAN c"test"
r"STDERR" GREATER THAN r"STDOUT"
r"STDERR" EQUAL r"STDERR"
