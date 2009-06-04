--TEST--
Bug #39018 (Error control operator '@' fails to suppress "Uninitialized string offset")
--FILE--
<?php

error_reporting(E_ALL);

$a = 'foo';
$a[11111111111];

$a = '';

$a[0];

print $a[0]; // 12

$a[-11111111111111111111111];

print $a[-11111111111111111111111]; // 16

$a[-0];

$x = 'test';

@$x[4];

@$y = $x[4];

@('a' == $x[4]);

$x[4] == 'a'; // 28

@$x[4] == 'a';

(@$x[4]) == 'a';

($x[4]) == 'a'; // 34

(@($x[4])) == 'a';

(($x[4])) == 'a'; // 38

@($x[4]) == 'a';

($x[4]) == 'a'; // 42

@($x[4] == 'a');

($x[4] == 'a'); // 46

$y = 'foobar';

$y[12.2];

print $y[12.2]; // 52

$y[3.5];

print $y[3.5]; // 56

print "\nDone\n";

?>
--EXPECTF--

Notice: Uninitialized string offset: 0 in %s on line 12

Notice: Uninitialized string offset: %i in %s on line 16

Notice: Uninitialized string offset: 4 in %s on line 28

Notice: Uninitialized string offset: 4 in %s on line 34

Notice: Uninitialized string offset: 4 in %s on line 38

Notice: Uninitialized string offset: 4 in %s on line 42

Notice: Uninitialized string offset: 4 in %s on line 46

Notice: Uninitialized string offset: 12 in %s on line 52
b
Done
