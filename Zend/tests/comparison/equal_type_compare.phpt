--TEST--
Compare types via == operator
--FILE--
<?php

function cmp($l, $r) {
	echo @match ($l == $r) {
		true => "EQUAL",
		false => "NOT EQUAL",
	};
}

class test {
	function __toString() {
		return "str";
	}
}

$values = [
	'null' => null,
	'false' => false,
	'true' => true,
	'i-5' => -5,
	'i0' => 0,
	'i1' => 1,
	'i10' => 10,
	'f-5' => -5.0,
	'f0' => 0.0,
	'f2.5' => 2.5,
	'f10' => 10.0,
	'fNAN' => NAN,
	's""' => "",
	's"str"' => "str",
	's"-5"' => "-5",
	's"0"' => "0",
	's"00"' => "00",
	's"0.0"' => "0.0",
	's"1"' => "1",
	's"2.5"' => "2.5",
	's"10"' => "10",
	'a[]' => [],
	'a[1, 2, 3]' => [1, 2, 3],
	'c"stdClass"' => new stdClass(),
	'c"test"' => new test(),
	'r"STDIN"' => STDIN,
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
null NOT EQUAL true
null NOT EQUAL i-5
null EQUAL i0
null NOT EQUAL i1
null NOT EQUAL i10
null NOT EQUAL f-5
null EQUAL f0
null NOT EQUAL f2.5
null NOT EQUAL f10
null NOT EQUAL fNAN
null EQUAL s""
null NOT EQUAL s"str"
null NOT EQUAL s"-5"
null NOT EQUAL s"0"
null NOT EQUAL s"00"
null NOT EQUAL s"0.0"
null NOT EQUAL s"1"
null NOT EQUAL s"2.5"
null NOT EQUAL s"10"
null EQUAL a[]
null NOT EQUAL a[1, 2, 3]
null NOT EQUAL c"stdClass"
null NOT EQUAL c"test"
null NOT EQUAL r"STDIN"
null NOT EQUAL r"STDERR"
false EQUAL null
false EQUAL false
false NOT EQUAL true
false NOT EQUAL i-5
false EQUAL i0
false NOT EQUAL i1
false NOT EQUAL i10
false NOT EQUAL f-5
false EQUAL f0
false NOT EQUAL f2.5
false NOT EQUAL f10
false NOT EQUAL fNAN
false EQUAL s""
false NOT EQUAL s"str"
false NOT EQUAL s"-5"
false EQUAL s"0"
false NOT EQUAL s"00"
false NOT EQUAL s"0.0"
false NOT EQUAL s"1"
false NOT EQUAL s"2.5"
false NOT EQUAL s"10"
false EQUAL a[]
false NOT EQUAL a[1, 2, 3]
false NOT EQUAL c"stdClass"
false NOT EQUAL c"test"
false NOT EQUAL r"STDIN"
false NOT EQUAL r"STDERR"
true NOT EQUAL null
true NOT EQUAL false
true EQUAL true
true EQUAL i-5
true NOT EQUAL i0
true EQUAL i1
true EQUAL i10
true EQUAL f-5
true NOT EQUAL f0
true EQUAL f2.5
true EQUAL f10
true EQUAL fNAN
true NOT EQUAL s""
true EQUAL s"str"
true EQUAL s"-5"
true NOT EQUAL s"0"
true EQUAL s"00"
true EQUAL s"0.0"
true EQUAL s"1"
true EQUAL s"2.5"
true EQUAL s"10"
true NOT EQUAL a[]
true EQUAL a[1, 2, 3]
true EQUAL c"stdClass"
true EQUAL c"test"
true EQUAL r"STDIN"
true EQUAL r"STDERR"
i-5 NOT EQUAL null
i-5 NOT EQUAL false
i-5 EQUAL true
i-5 EQUAL i-5
i-5 NOT EQUAL i0
i-5 NOT EQUAL i1
i-5 NOT EQUAL i10
i-5 EQUAL f-5
i-5 NOT EQUAL f0
i-5 NOT EQUAL f2.5
i-5 NOT EQUAL f10
i-5 NOT EQUAL fNAN
i-5 NOT EQUAL s""
i-5 NOT EQUAL s"str"
i-5 EQUAL s"-5"
i-5 NOT EQUAL s"0"
i-5 NOT EQUAL s"00"
i-5 NOT EQUAL s"0.0"
i-5 NOT EQUAL s"1"
i-5 NOT EQUAL s"2.5"
i-5 NOT EQUAL s"10"
i-5 NOT EQUAL a[]
i-5 NOT EQUAL a[1, 2, 3]
i-5 NOT EQUAL c"stdClass"
i-5 NOT EQUAL c"test"
i-5 NOT EQUAL r"STDIN"
i-5 NOT EQUAL r"STDERR"
i0 EQUAL null
i0 EQUAL false
i0 NOT EQUAL true
i0 NOT EQUAL i-5
i0 EQUAL i0
i0 NOT EQUAL i1
i0 NOT EQUAL i10
i0 NOT EQUAL f-5
i0 EQUAL f0
i0 NOT EQUAL f2.5
i0 NOT EQUAL f10
i0 NOT EQUAL fNAN
i0 NOT EQUAL s""
i0 NOT EQUAL s"str"
i0 NOT EQUAL s"-5"
i0 EQUAL s"0"
i0 EQUAL s"00"
i0 EQUAL s"0.0"
i0 NOT EQUAL s"1"
i0 NOT EQUAL s"2.5"
i0 NOT EQUAL s"10"
i0 NOT EQUAL a[]
i0 NOT EQUAL a[1, 2, 3]
i0 NOT EQUAL c"stdClass"
i0 NOT EQUAL c"test"
i0 NOT EQUAL r"STDIN"
i0 NOT EQUAL r"STDERR"
i1 NOT EQUAL null
i1 NOT EQUAL false
i1 EQUAL true
i1 NOT EQUAL i-5
i1 NOT EQUAL i0
i1 EQUAL i1
i1 NOT EQUAL i10
i1 NOT EQUAL f-5
i1 NOT EQUAL f0
i1 NOT EQUAL f2.5
i1 NOT EQUAL f10
i1 NOT EQUAL fNAN
i1 NOT EQUAL s""
i1 NOT EQUAL s"str"
i1 NOT EQUAL s"-5"
i1 NOT EQUAL s"0"
i1 NOT EQUAL s"00"
i1 NOT EQUAL s"0.0"
i1 EQUAL s"1"
i1 NOT EQUAL s"2.5"
i1 NOT EQUAL s"10"
i1 NOT EQUAL a[]
i1 NOT EQUAL a[1, 2, 3]
i1 EQUAL c"stdClass"
i1 EQUAL c"test"
i1 EQUAL r"STDIN"
i1 NOT EQUAL r"STDERR"
i10 NOT EQUAL null
i10 NOT EQUAL false
i10 EQUAL true
i10 NOT EQUAL i-5
i10 NOT EQUAL i0
i10 NOT EQUAL i1
i10 EQUAL i10
i10 NOT EQUAL f-5
i10 NOT EQUAL f0
i10 NOT EQUAL f2.5
i10 EQUAL f10
i10 NOT EQUAL fNAN
i10 NOT EQUAL s""
i10 NOT EQUAL s"str"
i10 NOT EQUAL s"-5"
i10 NOT EQUAL s"0"
i10 NOT EQUAL s"00"
i10 NOT EQUAL s"0.0"
i10 NOT EQUAL s"1"
i10 NOT EQUAL s"2.5"
i10 EQUAL s"10"
i10 NOT EQUAL a[]
i10 NOT EQUAL a[1, 2, 3]
i10 NOT EQUAL c"stdClass"
i10 NOT EQUAL c"test"
i10 NOT EQUAL r"STDIN"
i10 NOT EQUAL r"STDERR"
f-5 NOT EQUAL null
f-5 NOT EQUAL false
f-5 EQUAL true
f-5 EQUAL i-5
f-5 NOT EQUAL i0
f-5 NOT EQUAL i1
f-5 NOT EQUAL i10
f-5 EQUAL f-5
f-5 NOT EQUAL f0
f-5 NOT EQUAL f2.5
f-5 NOT EQUAL f10
f-5 NOT EQUAL fNAN
f-5 NOT EQUAL s""
f-5 NOT EQUAL s"str"
f-5 EQUAL s"-5"
f-5 NOT EQUAL s"0"
f-5 NOT EQUAL s"00"
f-5 NOT EQUAL s"0.0"
f-5 NOT EQUAL s"1"
f-5 NOT EQUAL s"2.5"
f-5 NOT EQUAL s"10"
f-5 NOT EQUAL a[]
f-5 NOT EQUAL a[1, 2, 3]
f-5 NOT EQUAL c"stdClass"
f-5 NOT EQUAL c"test"
f-5 NOT EQUAL r"STDIN"
f-5 NOT EQUAL r"STDERR"
f0 EQUAL null
f0 EQUAL false
f0 NOT EQUAL true
f0 NOT EQUAL i-5
f0 EQUAL i0
f0 NOT EQUAL i1
f0 NOT EQUAL i10
f0 NOT EQUAL f-5
f0 EQUAL f0
f0 NOT EQUAL f2.5
f0 NOT EQUAL f10
f0 NOT EQUAL fNAN
f0 NOT EQUAL s""
f0 NOT EQUAL s"str"
f0 NOT EQUAL s"-5"
f0 EQUAL s"0"
f0 EQUAL s"00"
f0 EQUAL s"0.0"
f0 NOT EQUAL s"1"
f0 NOT EQUAL s"2.5"
f0 NOT EQUAL s"10"
f0 NOT EQUAL a[]
f0 NOT EQUAL a[1, 2, 3]
f0 NOT EQUAL c"stdClass"
f0 NOT EQUAL c"test"
f0 NOT EQUAL r"STDIN"
f0 NOT EQUAL r"STDERR"
f2.5 NOT EQUAL null
f2.5 NOT EQUAL false
f2.5 EQUAL true
f2.5 NOT EQUAL i-5
f2.5 NOT EQUAL i0
f2.5 NOT EQUAL i1
f2.5 NOT EQUAL i10
f2.5 NOT EQUAL f-5
f2.5 NOT EQUAL f0
f2.5 EQUAL f2.5
f2.5 NOT EQUAL f10
f2.5 NOT EQUAL fNAN
f2.5 NOT EQUAL s""
f2.5 NOT EQUAL s"str"
f2.5 NOT EQUAL s"-5"
f2.5 NOT EQUAL s"0"
f2.5 NOT EQUAL s"00"
f2.5 NOT EQUAL s"0.0"
f2.5 NOT EQUAL s"1"
f2.5 EQUAL s"2.5"
f2.5 NOT EQUAL s"10"
f2.5 NOT EQUAL a[]
f2.5 NOT EQUAL a[1, 2, 3]
f2.5 NOT EQUAL c"stdClass"
f2.5 NOT EQUAL c"test"
f2.5 NOT EQUAL r"STDIN"
f2.5 NOT EQUAL r"STDERR"
f10 NOT EQUAL null
f10 NOT EQUAL false
f10 EQUAL true
f10 NOT EQUAL i-5
f10 NOT EQUAL i0
f10 NOT EQUAL i1
f10 EQUAL i10
f10 NOT EQUAL f-5
f10 NOT EQUAL f0
f10 NOT EQUAL f2.5
f10 EQUAL f10
f10 NOT EQUAL fNAN
f10 NOT EQUAL s""
f10 NOT EQUAL s"str"
f10 NOT EQUAL s"-5"
f10 NOT EQUAL s"0"
f10 NOT EQUAL s"00"
f10 NOT EQUAL s"0.0"
f10 NOT EQUAL s"1"
f10 NOT EQUAL s"2.5"
f10 EQUAL s"10"
f10 NOT EQUAL a[]
f10 NOT EQUAL a[1, 2, 3]
f10 NOT EQUAL c"stdClass"
f10 NOT EQUAL c"test"
f10 NOT EQUAL r"STDIN"
f10 NOT EQUAL r"STDERR"
fNAN NOT EQUAL null
fNAN NOT EQUAL false
fNAN EQUAL true
fNAN NOT EQUAL i-5
fNAN NOT EQUAL i0
fNAN NOT EQUAL i1
fNAN NOT EQUAL i10
fNAN NOT EQUAL f-5
fNAN NOT EQUAL f0
fNAN NOT EQUAL f2.5
fNAN NOT EQUAL f10
fNAN NOT EQUAL fNAN
fNAN NOT EQUAL s""
fNAN NOT EQUAL s"str"
fNAN NOT EQUAL s"-5"
fNAN NOT EQUAL s"0"
fNAN NOT EQUAL s"00"
fNAN NOT EQUAL s"0.0"
fNAN NOT EQUAL s"1"
fNAN NOT EQUAL s"2.5"
fNAN NOT EQUAL s"10"
fNAN NOT EQUAL a[]
fNAN NOT EQUAL a[1, 2, 3]
fNAN NOT EQUAL c"stdClass"
fNAN NOT EQUAL c"test"
fNAN NOT EQUAL r"STDIN"
fNAN NOT EQUAL r"STDERR"
s"" EQUAL null
s"" EQUAL false
s"" NOT EQUAL true
s"" NOT EQUAL i-5
s"" NOT EQUAL i0
s"" NOT EQUAL i1
s"" NOT EQUAL i10
s"" NOT EQUAL f-5
s"" NOT EQUAL f0
s"" NOT EQUAL f2.5
s"" NOT EQUAL f10
s"" NOT EQUAL fNAN
s"" EQUAL s""
s"" NOT EQUAL s"str"
s"" NOT EQUAL s"-5"
s"" NOT EQUAL s"0"
s"" NOT EQUAL s"00"
s"" NOT EQUAL s"0.0"
s"" NOT EQUAL s"1"
s"" NOT EQUAL s"2.5"
s"" NOT EQUAL s"10"
s"" NOT EQUAL a[]
s"" NOT EQUAL a[1, 2, 3]
s"" NOT EQUAL c"stdClass"
s"" NOT EQUAL c"test"
s"" NOT EQUAL r"STDIN"
s"" NOT EQUAL r"STDERR"
s"str" NOT EQUAL null
s"str" NOT EQUAL false
s"str" EQUAL true
s"str" NOT EQUAL i-5
s"str" NOT EQUAL i0
s"str" NOT EQUAL i1
s"str" NOT EQUAL i10
s"str" NOT EQUAL f-5
s"str" NOT EQUAL f0
s"str" NOT EQUAL f2.5
s"str" NOT EQUAL f10
s"str" NOT EQUAL fNAN
s"str" NOT EQUAL s""
s"str" EQUAL s"str"
s"str" NOT EQUAL s"-5"
s"str" NOT EQUAL s"0"
s"str" NOT EQUAL s"00"
s"str" NOT EQUAL s"0.0"
s"str" NOT EQUAL s"1"
s"str" NOT EQUAL s"2.5"
s"str" NOT EQUAL s"10"
s"str" NOT EQUAL a[]
s"str" NOT EQUAL a[1, 2, 3]
s"str" NOT EQUAL c"stdClass"
s"str" EQUAL c"test"
s"str" NOT EQUAL r"STDIN"
s"str" NOT EQUAL r"STDERR"
s"-5" NOT EQUAL null
s"-5" NOT EQUAL false
s"-5" EQUAL true
s"-5" EQUAL i-5
s"-5" NOT EQUAL i0
s"-5" NOT EQUAL i1
s"-5" NOT EQUAL i10
s"-5" EQUAL f-5
s"-5" NOT EQUAL f0
s"-5" NOT EQUAL f2.5
s"-5" NOT EQUAL f10
s"-5" NOT EQUAL fNAN
s"-5" NOT EQUAL s""
s"-5" NOT EQUAL s"str"
s"-5" EQUAL s"-5"
s"-5" NOT EQUAL s"0"
s"-5" NOT EQUAL s"00"
s"-5" NOT EQUAL s"0.0"
s"-5" NOT EQUAL s"1"
s"-5" NOT EQUAL s"2.5"
s"-5" NOT EQUAL s"10"
s"-5" NOT EQUAL a[]
s"-5" NOT EQUAL a[1, 2, 3]
s"-5" NOT EQUAL c"stdClass"
s"-5" NOT EQUAL c"test"
s"-5" NOT EQUAL r"STDIN"
s"-5" NOT EQUAL r"STDERR"
s"0" NOT EQUAL null
s"0" EQUAL false
s"0" NOT EQUAL true
s"0" NOT EQUAL i-5
s"0" EQUAL i0
s"0" NOT EQUAL i1
s"0" NOT EQUAL i10
s"0" NOT EQUAL f-5
s"0" EQUAL f0
s"0" NOT EQUAL f2.5
s"0" NOT EQUAL f10
s"0" NOT EQUAL fNAN
s"0" NOT EQUAL s""
s"0" NOT EQUAL s"str"
s"0" NOT EQUAL s"-5"
s"0" EQUAL s"0"
s"0" EQUAL s"00"
s"0" EQUAL s"0.0"
s"0" NOT EQUAL s"1"
s"0" NOT EQUAL s"2.5"
s"0" NOT EQUAL s"10"
s"0" NOT EQUAL a[]
s"0" NOT EQUAL a[1, 2, 3]
s"0" NOT EQUAL c"stdClass"
s"0" NOT EQUAL c"test"
s"0" NOT EQUAL r"STDIN"
s"0" NOT EQUAL r"STDERR"
s"00" NOT EQUAL null
s"00" NOT EQUAL false
s"00" EQUAL true
s"00" NOT EQUAL i-5
s"00" EQUAL i0
s"00" NOT EQUAL i1
s"00" NOT EQUAL i10
s"00" NOT EQUAL f-5
s"00" EQUAL f0
s"00" NOT EQUAL f2.5
s"00" NOT EQUAL f10
s"00" NOT EQUAL fNAN
s"00" NOT EQUAL s""
s"00" NOT EQUAL s"str"
s"00" NOT EQUAL s"-5"
s"00" EQUAL s"0"
s"00" EQUAL s"00"
s"00" EQUAL s"0.0"
s"00" NOT EQUAL s"1"
s"00" NOT EQUAL s"2.5"
s"00" NOT EQUAL s"10"
s"00" NOT EQUAL a[]
s"00" NOT EQUAL a[1, 2, 3]
s"00" NOT EQUAL c"stdClass"
s"00" NOT EQUAL c"test"
s"00" NOT EQUAL r"STDIN"
s"00" NOT EQUAL r"STDERR"
s"0.0" NOT EQUAL null
s"0.0" NOT EQUAL false
s"0.0" EQUAL true
s"0.0" NOT EQUAL i-5
s"0.0" EQUAL i0
s"0.0" NOT EQUAL i1
s"0.0" NOT EQUAL i10
s"0.0" NOT EQUAL f-5
s"0.0" EQUAL f0
s"0.0" NOT EQUAL f2.5
s"0.0" NOT EQUAL f10
s"0.0" NOT EQUAL fNAN
s"0.0" NOT EQUAL s""
s"0.0" NOT EQUAL s"str"
s"0.0" NOT EQUAL s"-5"
s"0.0" EQUAL s"0"
s"0.0" EQUAL s"00"
s"0.0" EQUAL s"0.0"
s"0.0" NOT EQUAL s"1"
s"0.0" NOT EQUAL s"2.5"
s"0.0" NOT EQUAL s"10"
s"0.0" NOT EQUAL a[]
s"0.0" NOT EQUAL a[1, 2, 3]
s"0.0" NOT EQUAL c"stdClass"
s"0.0" NOT EQUAL c"test"
s"0.0" NOT EQUAL r"STDIN"
s"0.0" NOT EQUAL r"STDERR"
s"1" NOT EQUAL null
s"1" NOT EQUAL false
s"1" EQUAL true
s"1" NOT EQUAL i-5
s"1" NOT EQUAL i0
s"1" EQUAL i1
s"1" NOT EQUAL i10
s"1" NOT EQUAL f-5
s"1" NOT EQUAL f0
s"1" NOT EQUAL f2.5
s"1" NOT EQUAL f10
s"1" NOT EQUAL fNAN
s"1" NOT EQUAL s""
s"1" NOT EQUAL s"str"
s"1" NOT EQUAL s"-5"
s"1" NOT EQUAL s"0"
s"1" NOT EQUAL s"00"
s"1" NOT EQUAL s"0.0"
s"1" EQUAL s"1"
s"1" NOT EQUAL s"2.5"
s"1" NOT EQUAL s"10"
s"1" NOT EQUAL a[]
s"1" NOT EQUAL a[1, 2, 3]
s"1" NOT EQUAL c"stdClass"
s"1" NOT EQUAL c"test"
s"1" EQUAL r"STDIN"
s"1" NOT EQUAL r"STDERR"
s"2.5" NOT EQUAL null
s"2.5" NOT EQUAL false
s"2.5" EQUAL true
s"2.5" NOT EQUAL i-5
s"2.5" NOT EQUAL i0
s"2.5" NOT EQUAL i1
s"2.5" NOT EQUAL i10
s"2.5" NOT EQUAL f-5
s"2.5" NOT EQUAL f0
s"2.5" EQUAL f2.5
s"2.5" NOT EQUAL f10
s"2.5" NOT EQUAL fNAN
s"2.5" NOT EQUAL s""
s"2.5" NOT EQUAL s"str"
s"2.5" NOT EQUAL s"-5"
s"2.5" NOT EQUAL s"0"
s"2.5" NOT EQUAL s"00"
s"2.5" NOT EQUAL s"0.0"
s"2.5" NOT EQUAL s"1"
s"2.5" EQUAL s"2.5"
s"2.5" NOT EQUAL s"10"
s"2.5" NOT EQUAL a[]
s"2.5" NOT EQUAL a[1, 2, 3]
s"2.5" NOT EQUAL c"stdClass"
s"2.5" NOT EQUAL c"test"
s"2.5" NOT EQUAL r"STDIN"
s"2.5" NOT EQUAL r"STDERR"
s"10" NOT EQUAL null
s"10" NOT EQUAL false
s"10" EQUAL true
s"10" NOT EQUAL i-5
s"10" NOT EQUAL i0
s"10" NOT EQUAL i1
s"10" EQUAL i10
s"10" NOT EQUAL f-5
s"10" NOT EQUAL f0
s"10" NOT EQUAL f2.5
s"10" EQUAL f10
s"10" NOT EQUAL fNAN
s"10" NOT EQUAL s""
s"10" NOT EQUAL s"str"
s"10" NOT EQUAL s"-5"
s"10" NOT EQUAL s"0"
s"10" NOT EQUAL s"00"
s"10" NOT EQUAL s"0.0"
s"10" NOT EQUAL s"1"
s"10" NOT EQUAL s"2.5"
s"10" EQUAL s"10"
s"10" NOT EQUAL a[]
s"10" NOT EQUAL a[1, 2, 3]
s"10" NOT EQUAL c"stdClass"
s"10" NOT EQUAL c"test"
s"10" NOT EQUAL r"STDIN"
s"10" NOT EQUAL r"STDERR"
a[] EQUAL null
a[] EQUAL false
a[] NOT EQUAL true
a[] NOT EQUAL i-5
a[] NOT EQUAL i0
a[] NOT EQUAL i1
a[] NOT EQUAL i10
a[] NOT EQUAL f-5
a[] NOT EQUAL f0
a[] NOT EQUAL f2.5
a[] NOT EQUAL f10
a[] NOT EQUAL fNAN
a[] NOT EQUAL s""
a[] NOT EQUAL s"str"
a[] NOT EQUAL s"-5"
a[] NOT EQUAL s"0"
a[] NOT EQUAL s"00"
a[] NOT EQUAL s"0.0"
a[] NOT EQUAL s"1"
a[] NOT EQUAL s"2.5"
a[] NOT EQUAL s"10"
a[] EQUAL a[]
a[] NOT EQUAL a[1, 2, 3]
a[] NOT EQUAL c"stdClass"
a[] NOT EQUAL c"test"
a[] NOT EQUAL r"STDIN"
a[] NOT EQUAL r"STDERR"
a[1, 2, 3] NOT EQUAL null
a[1, 2, 3] NOT EQUAL false
a[1, 2, 3] EQUAL true
a[1, 2, 3] NOT EQUAL i-5
a[1, 2, 3] NOT EQUAL i0
a[1, 2, 3] NOT EQUAL i1
a[1, 2, 3] NOT EQUAL i10
a[1, 2, 3] NOT EQUAL f-5
a[1, 2, 3] NOT EQUAL f0
a[1, 2, 3] NOT EQUAL f2.5
a[1, 2, 3] NOT EQUAL f10
a[1, 2, 3] NOT EQUAL fNAN
a[1, 2, 3] NOT EQUAL s""
a[1, 2, 3] NOT EQUAL s"str"
a[1, 2, 3] NOT EQUAL s"-5"
a[1, 2, 3] NOT EQUAL s"0"
a[1, 2, 3] NOT EQUAL s"00"
a[1, 2, 3] NOT EQUAL s"0.0"
a[1, 2, 3] NOT EQUAL s"1"
a[1, 2, 3] NOT EQUAL s"2.5"
a[1, 2, 3] NOT EQUAL s"10"
a[1, 2, 3] NOT EQUAL a[]
a[1, 2, 3] EQUAL a[1, 2, 3]
a[1, 2, 3] NOT EQUAL c"stdClass"
a[1, 2, 3] NOT EQUAL c"test"
a[1, 2, 3] NOT EQUAL r"STDIN"
a[1, 2, 3] NOT EQUAL r"STDERR"
c"stdClass" NOT EQUAL null
c"stdClass" NOT EQUAL false
c"stdClass" EQUAL true
c"stdClass" NOT EQUAL i-5
c"stdClass" NOT EQUAL i0
c"stdClass" EQUAL i1
c"stdClass" NOT EQUAL i10
c"stdClass" NOT EQUAL f-5
c"stdClass" NOT EQUAL f0
c"stdClass" NOT EQUAL f2.5
c"stdClass" NOT EQUAL f10
c"stdClass" NOT EQUAL fNAN
c"stdClass" NOT EQUAL s""
c"stdClass" NOT EQUAL s"str"
c"stdClass" NOT EQUAL s"-5"
c"stdClass" NOT EQUAL s"0"
c"stdClass" NOT EQUAL s"00"
c"stdClass" NOT EQUAL s"0.0"
c"stdClass" NOT EQUAL s"1"
c"stdClass" NOT EQUAL s"2.5"
c"stdClass" NOT EQUAL s"10"
c"stdClass" NOT EQUAL a[]
c"stdClass" NOT EQUAL a[1, 2, 3]
c"stdClass" EQUAL c"stdClass"
c"stdClass" NOT EQUAL c"test"
c"stdClass" NOT EQUAL r"STDIN"
c"stdClass" NOT EQUAL r"STDERR"
c"test" NOT EQUAL null
c"test" NOT EQUAL false
c"test" EQUAL true
c"test" NOT EQUAL i-5
c"test" NOT EQUAL i0
c"test" EQUAL i1
c"test" NOT EQUAL i10
c"test" NOT EQUAL f-5
c"test" NOT EQUAL f0
c"test" NOT EQUAL f2.5
c"test" NOT EQUAL f10
c"test" NOT EQUAL fNAN
c"test" NOT EQUAL s""
c"test" EQUAL s"str"
c"test" NOT EQUAL s"-5"
c"test" NOT EQUAL s"0"
c"test" NOT EQUAL s"00"
c"test" NOT EQUAL s"0.0"
c"test" NOT EQUAL s"1"
c"test" NOT EQUAL s"2.5"
c"test" NOT EQUAL s"10"
c"test" NOT EQUAL a[]
c"test" NOT EQUAL a[1, 2, 3]
c"test" NOT EQUAL c"stdClass"
c"test" EQUAL c"test"
c"test" NOT EQUAL r"STDIN"
c"test" NOT EQUAL r"STDERR"
r"STDIN" NOT EQUAL null
r"STDIN" NOT EQUAL false
r"STDIN" EQUAL true
r"STDIN" NOT EQUAL i-5
r"STDIN" NOT EQUAL i0
r"STDIN" EQUAL i1
r"STDIN" NOT EQUAL i10
r"STDIN" NOT EQUAL f-5
r"STDIN" NOT EQUAL f0
r"STDIN" NOT EQUAL f2.5
r"STDIN" NOT EQUAL f10
r"STDIN" NOT EQUAL fNAN
r"STDIN" NOT EQUAL s""
r"STDIN" NOT EQUAL s"str"
r"STDIN" NOT EQUAL s"-5"
r"STDIN" NOT EQUAL s"0"
r"STDIN" NOT EQUAL s"00"
r"STDIN" NOT EQUAL s"0.0"
r"STDIN" EQUAL s"1"
r"STDIN" NOT EQUAL s"2.5"
r"STDIN" NOT EQUAL s"10"
r"STDIN" NOT EQUAL a[]
r"STDIN" NOT EQUAL a[1, 2, 3]
r"STDIN" NOT EQUAL c"stdClass"
r"STDIN" NOT EQUAL c"test"
r"STDIN" EQUAL r"STDIN"
r"STDIN" NOT EQUAL r"STDERR"
r"STDERR" NOT EQUAL null
r"STDERR" NOT EQUAL false
r"STDERR" EQUAL true
r"STDERR" NOT EQUAL i-5
r"STDERR" NOT EQUAL i0
r"STDERR" NOT EQUAL i1
r"STDERR" NOT EQUAL i10
r"STDERR" NOT EQUAL f-5
r"STDERR" NOT EQUAL f0
r"STDERR" NOT EQUAL f2.5
r"STDERR" NOT EQUAL f10
r"STDERR" NOT EQUAL fNAN
r"STDERR" NOT EQUAL s""
r"STDERR" NOT EQUAL s"str"
r"STDERR" NOT EQUAL s"-5"
r"STDERR" NOT EQUAL s"0"
r"STDERR" NOT EQUAL s"00"
r"STDERR" NOT EQUAL s"0.0"
r"STDERR" NOT EQUAL s"1"
r"STDERR" NOT EQUAL s"2.5"
r"STDERR" NOT EQUAL s"10"
r"STDERR" NOT EQUAL a[]
r"STDERR" NOT EQUAL a[1, 2, 3]
r"STDERR" NOT EQUAL c"stdClass"
r"STDERR" NOT EQUAL c"test"
r"STDERR" NOT EQUAL r"STDIN"
r"STDERR" EQUAL r"STDERR"
