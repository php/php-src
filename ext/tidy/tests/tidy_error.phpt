--TEST--
Ensure tidy_get_status() returns correct status
--CREDITS--
Stefan Priebsch
--SKIPIF--
<?php
 if (!extension_loaded("tidy")) print "skip tidy extension not loaded";
?>
--FILE--
<?php

$html = '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">
<html>
<head>
<title></title>
</head>
<body>
<p>paragraph</p>
</body>
</html>';
$tidy = tidy_parse_string($html);

echo tidy_get_status($tidy);
// status 0 indicates no errors or warnings

$html = '<p>paragraph</i>';
$tidy = tidy_parse_string($html);

echo tidy_get_status($tidy);
// status 1 indicates warnings

$html = '<bogus>test</bogus>';
$tidy = tidy_parse_string($html);

echo tidy_get_status($tidy);
// status 2 indicates error

?>
--EXPECT--
012
