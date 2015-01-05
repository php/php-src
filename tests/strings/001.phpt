--TEST--
String functions
--FILE--
<?php 

error_reporting(0);

echo "Testing strtok: ";

$str = "testing 1/2\\3";
$tok1 = strtok($str, " ");
$tok2 = strtok("/");
$tok3 = strtok("\\");
$tok4 = strtok(".");
if ($tok1 != "testing") {
	echo("failed 1\n");
} elseif ($tok2 != "1") {
	echo("failed 2\n");
} elseif ($tok3 != "2") {
	echo("failed 3\n");
} elseif ($tok4 != "3") {
	echo("failed 4\n");
} else {
	echo("passed\n");
}

echo "Testing strstr: ";
$test = "This is a test";
$found1 = strstr($test, 32);
$found2 = strstr($test, "a ");
if ($found1 != " is a test") {
	echo("failed 1\n");
} elseif ($found2 != "a test") {
	echo("failed 2\n");
} else {
	echo("passed\n");
}

echo "Testing strrchr: ";
$test = "fola fola blakken";
$found1 = strrchr($test, "b");
$found2 = strrchr($test, 102);
if ($found1 != "blakken") {
	echo("failed 1\n");
} elseif ($found2 != "fola blakken") {
	echo("failed 2\n");
}
else {
	echo("passed\n");
}

echo "Testing strtoupper: ";
$test = "abCdEfg";
$upper = strtoupper($test);
if ($upper == "ABCDEFG") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing strtolower: ";
$test = "ABcDeFG";
$lower = strtolower($test);
if ($lower == "abcdefg") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing substr: ";
$tests = $ok = 0;
$string = "string12345";
$tests++; if (substr($string, 2, 10) == "ring12345") { $ok++; }
$tests++; if (substr($string, 4, 7) == "ng12345") { $ok++; }
$tests++; if (substr($string, 4) == "ng12345") { $ok++; }
$tests++; if (substr($string, 10, 2) == "5") { $ok++; }
$tests++; if (substr($string, 6, 0) == "") { $ok++; }
$tests++; if (substr($string, -2, 2) == "45") { $ok++; }
$tests++; if (substr($string, 1, -1) == "tring1234") { $ok++; }
$tests++; if (substr($string, -1, -2) == "") { $ok++; }
$tests++; if (substr($string, -3, -2) == "3") { $ok++; }

if ($tests == $ok) {
	echo("passed\n");
} else {
	echo("failed!\n");
}

$raw = ' !"#$%&\'()*+,-./0123456789:;<=>?'
     . '@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'
     . '`abcdefghijklmnopqrstuvwxyz{|}~'
     . "\0";

echo "Testing rawurlencode: ";
$encoded = rawurlencode($raw);
$correct = '%20%21%22%23%24%25%26%27%28%29%2A%2B%2C-.%2F0123456789%3A%3B%3C%3D%3E%3F'
         . '%40ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_'
         . '%60abcdefghijklmnopqrstuvwxyz%7B%7C%7D~'
         . '%00';
if ($encoded == $correct) {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing rawurldecode: ";
$decoded = rawurldecode($correct);
if ($decoded == $raw) {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing urlencode: ";
$encoded = urlencode($raw);
$correct = '+%21%22%23%24%25%26%27%28%29%2A%2B%2C-.%2F0123456789%3A%3B%3C%3D%3E%3F'
         . '%40ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_'
         . '%60abcdefghijklmnopqrstuvwxyz%7B%7C%7D%7E'
         . '%00';
if ($encoded == $correct) {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing urldecode: ";
$decoded = urldecode($correct);
if ($decoded == $raw) {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing quotemeta: ";
$raw = "a.\\+*?" . chr(91) . "^" . chr(93) . "b\$c";
$quoted = quotemeta($raw);
if ($quoted == "a\\.\\\\\\+\\*\\?\\[\\^\\]b\\\$c") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing ufirst: ";
$str = "fahrvergnuegen";
$uc = ucfirst($str);
if ($uc == "Fahrvergnuegen") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing strtr: ";
$str = "test abcdefgh";
$tr = strtr($str, "def", "456");
if ($tr == "t5st abc456gh") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing addslashes: ";
$str = "\"\\'";
$as = addslashes($str);
if ($as == "\\\"\\\\\\'") {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing stripslashes: ";
$str = "\$\\'";
$ss = stripslashes($str);
if ($ss == "\$'") {
	echo("passed\n");
} else {
	echo("failed!\n");
}


echo "Testing uniqid(true): ";
$str = "prefix";
$ui1 = uniqid($str, true);
$ui2 = uniqid($str, true);

$len = 29;

if (strlen($ui1) == strlen($ui2) && strlen($ui1) == $len && $ui1 != $ui2) {
	echo("passed\n");
} else {
	echo("failed!\n");
}

echo "Testing uniqid(false): ";
$str = "prefix";
$ui1 = uniqid($str);
usleep( 1 );
$ui2 = uniqid($str);

$len = strncasecmp(PHP_OS, 'CYGWIN', 6) ? 19 : 29;

if (strlen($ui1) == strlen($ui2) && strlen($ui1) == $len && $ui1 != $ui2) {
	echo("passed\n");
} else {
	echo("failed!\n");
}

?>
--EXPECT--
Testing strtok: passed
Testing strstr: passed
Testing strrchr: passed
Testing strtoupper: passed
Testing strtolower: passed
Testing substr: passed
Testing rawurlencode: passed
Testing rawurldecode: passed
Testing urlencode: passed
Testing urldecode: passed
Testing quotemeta: passed
Testing ufirst: passed
Testing strtr: passed
Testing addslashes: passed
Testing stripslashes: passed
Testing uniqid(true): passed
Testing uniqid(false): passed
