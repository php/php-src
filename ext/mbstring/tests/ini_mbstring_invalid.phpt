--TEST--
Invalid values for MBString INI settings
--EXTENSIONS--
mbstring
--INI--
mbstring.language=UNKNOWN_LANGUAGE
mbstring.internal_encoding=UNKNOWN_ENCODING
mbstring.detect_order=UTF-8,DETECT_ORDER,ASCII
mbstring.http_input=UTF-8,HTTP_INPUT,ASCII
mbstring.http_output=HTTP_OUTPUT
mbstring.http_output_conv_mimetypes=UNKNOWN_MIME_TYPE_OUTPUT
mbstring.substitute_character=U+3000,NON_EXISTING_CHARACTER,JIS+7E7E
mbstring.encoding_translation=BOOL_TRANSLATION
mbstring.strict_detection=BOOL_STRICT_DETECTION
--FILE--
<?php
// Empty as we are only testing INI settings
?>
--EXPECT--
Warning: PHP Startup: INI setting contains invalid encoding "DETECT_ORDER" in Unknown on line 0

Deprecated: PHP Startup: Use of mbstring.http_input is deprecated in Unknown on line 0

Warning: PHP Startup: INI setting contains invalid encoding "HTTP_INPUT" in Unknown on line 0

Deprecated: PHP Startup: Use of mbstring.http_output is deprecated in Unknown on line 0

Deprecated: PHP Startup: Use of mbstring.internal_encoding is deprecated in Unknown on line 0

Warning: PHP Startup: Unknown encoding "UNKNOWN_ENCODING" in ini setting in Unknown on line 0
