--TEST--
Bug #32589 (crash inside imap_mail_compose() function)
--SKIPIF--
<?php
        if (!extension_loaded("imap")) {
                die("skip imap extension not available");
        }
?>
--FILE--
<?php
$m_envelope["To"] = "mail@example.com";
$m_part1["type"] = TYPEMULTIPART;
$m_part1["subtype"] = "mixed";
$m_part2["type"] = TYPETEXT;
$m_part2["subtype"] = "plain";
$m_part2["description"] = "text_message";

$m_part2["charset"] = "ISO-8859-2";

$m_part2["contents.data"] = "hello";
$m_body[1] = $m_part1;
$m_body[2] = $m_part2;
echo imap_mail_compose($m_envelope, $m_body);
?>
--EXPECTF--
MIME-Version: 1.0
Content-Type: MULTIPART/mixed; BOUNDARY="%s"

%s
Content-Type: TEXT/plain; CHARSET=ISO-8859-2
Content-Description: text_message

hello
%s
