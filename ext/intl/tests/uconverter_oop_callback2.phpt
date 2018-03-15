--TEST--
UConverter::convert() w/ Callback Reasons
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '60.1') < 0) die('skip for ICU >= 60.1'); ?>
--FILE--
<?php
class MyConverter extends UConverter {
  /**
   * Called during conversion from source encoding to internal UChar representation
   */
  public function toUCallback($reason, $source, $codeUnits, &$error) {
    echo "toUCallback(", UConverter::reasonText($reason), ", ...)\n";
    return parent::toUCallback($reason, $source, $codeUnits, $error);
  }

  /**
   * Called during conversion from internal UChar to destination encoding
   */
  public function fromUCallback($reason, $source, $codePoint, &$error) {
    echo "fromUCallback(", UConverter::reasonText($reason), ", ...)\n";
    return parent::fromUCallback($reason, $source, $codePoint, $error);
  }

}

$c = new MyConverter('ascii', 'utf-8');
foreach(array("regular", "irregul\xC1\xA1r", "\xC2\xA1unsupported!") as $word) {
  $c->convert($word);
}
--EXPECT--
toUCallback(REASON_RESET, ...)
toUCallback(REASON_RESET, ...)
fromUCallback(REASON_RESET, ...)
fromUCallback(REASON_RESET, ...)
toUCallback(REASON_RESET, ...)
toUCallback(REASON_ILLEGAL, ...)
toUCallback(REASON_ILLEGAL, ...)
toUCallback(REASON_RESET, ...)
toUCallback(REASON_ILLEGAL, ...)
toUCallback(REASON_ILLEGAL, ...)
fromUCallback(REASON_RESET, ...)
fromUCallback(REASON_UNASSIGNED, ...)
fromUCallback(REASON_UNASSIGNED, ...)
fromUCallback(REASON_RESET, ...)
fromUCallback(REASON_UNASSIGNED, ...)
fromUCallback(REASON_UNASSIGNED, ...)
toUCallback(REASON_RESET, ...)
toUCallback(REASON_RESET, ...)
fromUCallback(REASON_RESET, ...)
fromUCallback(REASON_UNASSIGNED, ...)
fromUCallback(REASON_RESET, ...)
fromUCallback(REASON_UNASSIGNED, ...)
toUCallback(REASON_CLOSE, ...)
fromUCallback(REASON_CLOSE, ...)
toUCallback(REASON_CLOSE, ...)
fromUCallback(REASON_CLOSE, ...)
