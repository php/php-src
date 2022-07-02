--TEST--
Bug #63839 (iconv_mime_decode_headers function is skipping headers)
--EXTENSIONS--
iconv
--FILE--
<?php
$headers = 'From: "xyz" <xyz@xyz.com>
To: <xyz@xyz.com>
Subject: Reply Is? white side-LED =? in Help
Date: Sat, 22 Dec 2012
Message-ID: <006f01cde00e$d9f79da0$8de6d8e0>
MIME-Version: 1.0
Content-Type: multipart/alternative;
    boundary="----=_NextPart_000_0070_01CDE03C.F3AFD9A0"
X-Mailer: Microsoft Office Outlook 12.0
Thread-Index: Ac3gDtcH2huHjzYcQVmFJPPoWjJogA==
Content-Language: en-us

';
var_dump(iconv_mime_decode_headers($headers, ICONV_MIME_DECODE_CONTINUE_ON_ERROR));
var_dump(iconv_mime_decode_headers($headers, ICONV_MIME_DECODE_STRICT));
?>
--EXPECT--
array(10) {
  ["From"]=>
  string(19) ""xyz" <xyz@xyz.com>"
  ["To"]=>
  string(13) "<xyz@xyz.com>"
  ["Subject"]=>
  string(35) "Reply Is? white side-LED =? in Help"
  ["Date"]=>
  string(16) "Sat, 22 Dec 2012"
  ["Message-ID"]=>
  string(32) "<006f01cde00e$d9f79da0$8de6d8e0>"
  ["MIME-Version"]=>
  string(3) "1.0"
  ["Content-Type"]=>
  string(75) "multipart/alternative; boundary="----=_NextPart_000_0070_01CDE03C.F3AFD9A0""
  ["X-Mailer"]=>
  string(29) "Microsoft Office Outlook 12.0"
  ["Thread-Index"]=>
  string(32) "Ac3gDtcH2huHjzYcQVmFJPPoWjJogA=="
  ["Content-Language"]=>
  string(5) "en-us"
}
array(10) {
  ["From"]=>
  string(19) ""xyz" <xyz@xyz.com>"
  ["To"]=>
  string(13) "<xyz@xyz.com>"
  ["Subject"]=>
  string(35) "Reply Is? white side-LED =? in Help"
  ["Date"]=>
  string(16) "Sat, 22 Dec 2012"
  ["Message-ID"]=>
  string(32) "<006f01cde00e$d9f79da0$8de6d8e0>"
  ["MIME-Version"]=>
  string(3) "1.0"
  ["Content-Type"]=>
  string(75) "multipart/alternative; boundary="----=_NextPart_000_0070_01CDE03C.F3AFD9A0""
  ["X-Mailer"]=>
  string(29) "Microsoft Office Outlook 12.0"
  ["Thread-Index"]=>
  string(32) "Ac3gDtcH2huHjzYcQVmFJPPoWjJogA=="
  ["Content-Language"]=>
  string(5) "en-us"
}
