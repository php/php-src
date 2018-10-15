--TEST--
crypt() SHA-512
--SKIPIF--
<?php
if (!function_exists('crypt') || !defined("CRYPT_SHA512")) {
	die("SKIP crypt()-sha512 is not available");
}
?>
--FILE--
<?php

$tests = array(
	1 => array(
		'$6$saltstring',
		'Hello world!',
		'$6$saltstring$svn8UoSVapNtMuq1ukKS4tPQd8iKwSMHWjl/O817G3uBnIFNjnQJuesI68u4OTLiBFdcbYEdFCoEOfaS35inz1'
	),
	2 => array(
		'$6$rounds=10000$saltstringsaltstring',
		'Hello world!',
		'$6$rounds=10000$saltstringsaltst$OW1/O6BYHV6BcXZu8QVeXbDWra3Oeqh0sbHbbMCVNSnCM/UrjmM0Dp8vOuZeHBy/YTBmSK6H9qs/y3RnOaw5v.'
	),
	3 => array(
		'$6$rounds=5000$toolongsaltstring',
		'This is just a test',
		'$6$rounds=5000$toolongsaltstrin$lQ8jolhgVRVhY4b5pZKaysCLi0QBxGoNeKQzQ3glMhwllF7oGDZxUhx1yxdYcz/e1JSbq3y6JMxxl8audkUEm0'
	),
	4 => array(
		'$6$rounds=1400$anotherlongsaltstring',
		'a very much longer text to encrypt.  This one even stretches over morethan one line.',
		'$6$rounds=1400$anotherlongsalts$POfYwTEok97VWcjxIiSOjiykti.o/pQs.wPvMxQ6Fm7I6IoYN3CmLs66x9t0oSwbtEW7o7UmJEiDwGqd8p4ur1'
	),
	5 => array(
		'$6$rounds=77777$short',
		'we have a short salt string but not a short password',
		'$6$rounds=77777$short$WuQyW2YR.hBNpjjRhpYD/ifIw05xdfeEyQoMxIXbkvr0gge1a1x3yRULJ5CCaUeOxFmtlcGZelFl5CxtgfiAc0'
	),
	6 => array(
		'$6$rounds=123456$asaltof16chars..',
		'a short string',
		'$6$rounds=123456$asaltof16chars..$BtCwjqMJGx5hrJhZywWvt0RLE8uZ4oPwcelCjmw2kSYu.Ec6ycULevoBK25fs2xXgMNrCzIMVcgEJAstJeonj1'
	),
	7 => array(
		'$6$rounds=10$roundstoolow',
		'the minimum number is still observed',
		'$6$rounds=1000$roundstoolow$kUMsbe306n21p9R.FRkW3IGn.S9NPN0x50YhH1xhLsPuWGsUSklZt58jaTfF4ZEQpyUNGc0dqbpBYYBaHHrsX.'
	),
	8 => array(
		'$6$$bar$',
		'foo',
		'$6$$QMXjqd7rHQZPQ1yHsXkQqC1FBzDiVfTHXL.LaeDAeVV.IzMaV9VU4MQ8kPuZa2SOP1A0RPm772EaFYjpEJtdu.'
	),
);

foreach ($tests as $iter => $t) {
	$res = crypt($t[1], $t[0]);
	if ($res != $t[2]) echo "Iteration $iter failed.
Expected: <$t[2]>
Got       <$res>\n";
}
echo "Passes.";
?>
--EXPECTF--
Passes.
