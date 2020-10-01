--TEST--
Bug #61116 (HTML functions use encoding, not charset)
--FILE--
<?php
echo new ReflectionFunction('htmlspecialchars'), "\n";
echo new ReflectionFunction('get_html_translation_table'), "\n";
?>
--EXPECT--
Function [ <internal:standard> function htmlspecialchars ] {

  - Parameters [4] {
    Parameter #0 [ <required> string $string ]
    Parameter #1 [ <optional> int $flags = ENT_COMPAT ]
    Parameter #2 [ <optional> ?string $encoding = null ]
    Parameter #3 [ <optional> bool $double_encode = true ]
  }
  - Return [ string ]
}

Function [ <internal:standard> function get_html_translation_table ] {

  - Parameters [3] {
    Parameter #0 [ <optional> int $table = HTML_SPECIALCHARS ]
    Parameter #1 [ <optional> int $flags = ENT_COMPAT ]
    Parameter #2 [ <optional> string $encoding = "UTF-8" ]
  }
  - Return [ array ]
}
