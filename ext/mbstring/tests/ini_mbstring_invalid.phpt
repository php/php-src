--TEST--
Invalid values for MBString INI settings
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.language=UNKNOWN_LANGUAGE
mbstring.detect_order=UTF-8,DETECT_ORDER,ASCII
mbstring.http_output_conv_mimetypes=UNKNOWN_MIME_TYPE_OUTPUT
mbstring.substitute_character=U+3000,NON_EXISTING_CHARACTER,JIS+7E7E
mbstring.func_overload=BOOL_OVERLOAD
mbstring.encoding_translation=BOOL_TRANSLATION
mbstring.strict_detection=BOOL_STRICT_DETECTION
--FILE--
<?php
// Empty as we are only testing INI settings
?>
--EXPECT--
PHP Warning:  PHP Startup: INI setting contains invalid encoding "DETECT_ORDER" in Unknown on line 0

Warning: PHP Startup: INI setting contains invalid encoding "DETECT_ORDER" in Unknown on line 0
