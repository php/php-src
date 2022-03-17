--TEST--
GH-8208 (mb_encode_mimeheader: $indent functionality broken)
--EXTENSIONS--
mbstring
--FILE--
<?php
$s = "[service-Aufgaben S&W-Team][#19415] VM''s aufsetzen mit unterschiedlichen";
$p = 'Subject: ';
var_dump(
    $p . mb_encode_mimeheader($s, 'UTF-8', 'Q', "\015\012", strlen($p)),
    mb_encode_mimeheader($p . $s, 'UTF-8', 'Q', "\015\012", 0)
);
?>
--EXPECT--
string(84) "Subject: [service-Aufgaben S&W-Team][#19415] VM''s aufsetzen mit
 unterschiedlichen"
string(84) "Subject: [service-Aufgaben S&W-Team][#19415] VM''s aufsetzen mit
 unterschiedlichen"
