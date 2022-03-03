--TEST--
IntlChar basic functionality
--EXTENSIONS--
intl
--FILE--
<?php

function unicode_info($cp) {
  $proplist = array(
    IntlChar::PROPERTY_ALPHABETIC,
  );
  $methodList = array(
    'isUAlphabetic',
    'isUUppercase', 'isupper',
    'isULowercase', 'islower',
    'isUWhiteSpace', 'isWhitespace',
    'istitle', 'isdigit', 'isalpha', 'isalnum',
    'isxdigit', 'ispunct', 'ispunct', 'isgraph',
    'isblank', 'isdefined', 'isspace', 'iscntrl',
    'isMirrored', 'isIDStart', 'isIDPart',
    'getBlockCode', 'charName',
  );

  $ncp = IntlChar::ord($cp);
  printf("Codepoint U+%04x\n", $ncp);

  foreach($proplist as $prop) {
    printf("  hasBinaryProperty(%s): %s\n",
      IntlChar::getPropertyName($prop),
      IntlChar::hasBinaryProperty($cp, $prop) ? "true" : "false"
    );
  }
  foreach($methodList as $method) {
    echo "  $method(): ";
    var_dump(IntlChar::$method($cp));
  }
  echo "  charAge(): ", implode('.', IntlChar::charAge($cp)), "\n";
  echo "\n";
}

printf("Codepoint range: %04x-%04x\n", IntlChar::CODEPOINT_MIN, IntlChar::CODEPOINT_MAX);
$codepoints = array('P', 0xDF, 0x2603);
foreach($codepoints as $cp) {
  unicode_info($cp);
}
echo "Sample range of codepoints: U+2600-U+260F\n";
IntlChar::enumCharNames(0x2600, 0x2610, function($cp, $nc, $name) {
  printf("U+%04x %s\n", $cp, $name);
});
echo "RECYCLING SYMBOL FOR TYPE-1 PLASTICS => ";
var_dump(IntlChar::charFromName("RECYCLING SYMBOL FOR TYPE-1 PLASTICS"));
?>
--EXPECT--
Codepoint range: 0000-10ffff
Codepoint U+0050
  hasBinaryProperty(Alphabetic): true
  isUAlphabetic(): bool(true)
  isUUppercase(): bool(true)
  isupper(): bool(true)
  isULowercase(): bool(false)
  islower(): bool(false)
  isUWhiteSpace(): bool(false)
  isWhitespace(): bool(false)
  istitle(): bool(false)
  isdigit(): bool(false)
  isalpha(): bool(true)
  isalnum(): bool(true)
  isxdigit(): bool(false)
  ispunct(): bool(false)
  ispunct(): bool(false)
  isgraph(): bool(true)
  isblank(): bool(false)
  isdefined(): bool(true)
  isspace(): bool(false)
  iscntrl(): bool(false)
  isMirrored(): bool(false)
  isIDStart(): bool(true)
  isIDPart(): bool(true)
  getBlockCode(): int(1)
  charName(): string(22) "LATIN CAPITAL LETTER P"
  charAge(): 1.1.0.0

Codepoint U+00df
  hasBinaryProperty(Alphabetic): true
  isUAlphabetic(): bool(true)
  isUUppercase(): bool(false)
  isupper(): bool(false)
  isULowercase(): bool(true)
  islower(): bool(true)
  isUWhiteSpace(): bool(false)
  isWhitespace(): bool(false)
  istitle(): bool(false)
  isdigit(): bool(false)
  isalpha(): bool(true)
  isalnum(): bool(true)
  isxdigit(): bool(false)
  ispunct(): bool(false)
  ispunct(): bool(false)
  isgraph(): bool(true)
  isblank(): bool(false)
  isdefined(): bool(true)
  isspace(): bool(false)
  iscntrl(): bool(false)
  isMirrored(): bool(false)
  isIDStart(): bool(true)
  isIDPart(): bool(true)
  getBlockCode(): int(2)
  charName(): string(26) "LATIN SMALL LETTER SHARP S"
  charAge(): 1.1.0.0

Codepoint U+2603
  hasBinaryProperty(Alphabetic): false
  isUAlphabetic(): bool(false)
  isUUppercase(): bool(false)
  isupper(): bool(false)
  isULowercase(): bool(false)
  islower(): bool(false)
  isUWhiteSpace(): bool(false)
  isWhitespace(): bool(false)
  istitle(): bool(false)
  isdigit(): bool(false)
  isalpha(): bool(false)
  isalnum(): bool(false)
  isxdigit(): bool(false)
  ispunct(): bool(false)
  ispunct(): bool(false)
  isgraph(): bool(true)
  isblank(): bool(false)
  isdefined(): bool(true)
  isspace(): bool(false)
  iscntrl(): bool(false)
  isMirrored(): bool(false)
  isIDStart(): bool(false)
  isIDPart(): bool(false)
  getBlockCode(): int(55)
  charName(): string(7) "SNOWMAN"
  charAge(): 1.1.0.0

Sample range of codepoints: U+2600-U+260F
U+2600 BLACK SUN WITH RAYS
U+2601 CLOUD
U+2602 UMBRELLA
U+2603 SNOWMAN
U+2604 COMET
U+2605 BLACK STAR
U+2606 WHITE STAR
U+2607 LIGHTNING
U+2608 THUNDERSTORM
U+2609 SUN
U+260a ASCENDING NODE
U+260b DESCENDING NODE
U+260c CONJUNCTION
U+260d OPPOSITION
U+260e BLACK TELEPHONE
U+260f WHITE TELEPHONE
RECYCLING SYMBOL FOR TYPE-1 PLASTICS => int(9843)
