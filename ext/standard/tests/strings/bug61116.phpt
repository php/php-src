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
    Parameter #0 [ <required> $string ]
    Parameter #1 [ <optional> $quote_style ]
    Parameter #2 [ <optional> $encoding ]
    Parameter #3 [ <optional> $double_encode ]
  }
}

Function [ <internal:standard> function get_html_translation_table ] {

  - Parameters [3] {
    Parameter #0 [ <optional> $table ]
    Parameter #1 [ <optional> $quote_style ]
    Parameter #2 [ <optional> $encoding ]
  }
}
