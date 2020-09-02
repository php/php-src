--TEST--
UConverter::convert() w/ Callback Return Values
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
class MyConverter extends UConverter {
  public function toUCallback($reason, $source, $codeUnits, &$error) {
    $error = U_ZERO_ERROR;
    switch ($codeUnits) {
      case "\x80": return NULL;
      case "\x81": return 'a';
      case "\x82": return ord('b');
      case "\x83": return array('c');
    }
  }

  /**
   * Called during conversion from internal UChar to destination encoding
   */
  public function fromUCallback($reason, $source, $codePoint, &$error) {
    $error = U_ZERO_ERROR;
    switch ($codePoint) {
      case 0x00F1: return "A";
      case 0x00F2: return ord("B");
      case 0x00F3: return array("C");
      case 0x00F4: return NULL;
    }
  }

}

$c = new MyConverter('ascii', 'utf-8');
// This line will trigger toUCallback
var_dump($c->convert("\x80\x81\x82\x83"));
// This line will trigger fromUCallback
var_dump($c->convert("\xC3\xB1\xC3\xB2\xC3\xB3\xC3\xB4"));
?>
--EXPECT--
string(3) "abc"
string(3) "ABC"
