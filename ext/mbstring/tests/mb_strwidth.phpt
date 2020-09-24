--TEST--
mb_strwidth()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
ini_set('include_path', __DIR__);
include_once('common.inc');

// EUC-JP
$euc_jp  = pack('H*', '30313233a4b3a4cecab8bbfacef3a4cfc6fccbdcb8eca4c7a4b9a1a34555432d4a50a4f2bbc8a4c3a4c6a4a4a4dea4b9a1a3c6fccbdcb8eca4cfccccc5ddbdada4a4a1a3');
// UTF-8
$utf8    = '∮ E⋅da = Q';
// UTF-16LE
$utf16le = pack('H*', '1a043804400438043b043b04380446043004200069007300200043007900720069006c006c0069006300');
// UTF-16BE
$utf16be = pack('H*', '041a043804400438043b043b04380446043000200069007300200043007900720069006c006c00690063');
// KOI8-R
$koi8r   = pack('H*', 'ebc9d2c9ccccc9c3c120697320437972696c6c6963');

print "1: " . mb_strwidth($euc_jp,  'EUC-JP')   . "\n";
print "2: " . mb_strwidth($utf8,    'UTF-8')    . "\n";
print "3: " . mb_strwidth($utf16le, 'UTF-16LE') . "\n";
print "4: " . mb_strwidth($utf16be, 'UTF-16BE') . "\n";
print "5: " . mb_strwidth($koi8r,   'KOI8-R')   . "\n";

// OK, that was just starters. Now let's get real.

$wideChars   = [];
$narrowChars = [];

function storeCharByWidth($codepoint, $eawCode) {
  global $wideChars, $narrowChars;
  // Interpret code from EastAsianWidth.txt data file (from Unicode consortium)
  if ($eawCode == 'W' || $eawCode == 'F') {
    // 'Wide' or 'Fullwidth'
    array_push($wideChars, pack('N', $codepoint));
  } else if ($eawCode == 'H' || $eawCode == 'Na' || $eawCode == 'N') {
    // 'Halfwidth', 'Narrow', or 'Neutral'
    array_push($narrowChars, pack('N', $codepoint));
  }
  // The other possible code is 'A' for 'Ambiguous'
  // We don't do any testing on those
}

$fp = fopen(__DIR__ . '/data/EastAsianWidth.txt', 'r+');
while ($line = fgets($fp, 256)) {
  if ($line[0] == '#')
    continue;

  if (sscanf($line, '%x..%x;%s', $startCodepoint, $endCodepoint, $eaw) == 3) {
    for ($i = $startCodepoint; $i <= $endCodepoint; $i++)
      storeCharByWidth($i, $eaw);
  } else if (sscanf($line, '%x;%s', $codepoint, $eaw) == 2) {
    storeCharByWidth($codepoint, $eaw);
  }
}

foreach ($wideChars as $wideChar) {
  if (mb_strwidth($wideChar, 'UTF-32BE') != 2) {
    die("Oops! Codepoint " . bin2hex($wideChar) . " should have mb_strwidth of 2");
  }
}
echo "mb_strwidth works as expected on all 'wide' characters\n";

foreach ($narrowChars as $narrowChar) {
  if (mb_strwidth($narrowChar, 'UTF-32BE') != 1) {
    die("Oops! Codepoint " . bin2hex($narrowChar) . " should have mb_strwidth of 1");
  }
}
echo "mb_strwidth works as expected on all 'narrow' characters\n";

// Now try some random combinations
srand(123); // Make results consistent
shuffle($wideChars);
shuffle($narrowChars);

for ($i = 0; $i < 100; $i++) {
  // Put together a random combination of wide and narrow chars
  $length = rand(5, 20);
  $testString = '';
  $testWidth  = 0;
  while ($length--) {
    if (rand(0, 1) == 1) {
      $testString .= array_pop($wideChars);
      $testWidth += 2;
    } else {
      $testString .= array_pop($narrowChars);
      $testWidth += 1;
    }
  }
  if (mb_strwidth($testString, 'UTF-32BE') != $testWidth)
    die("mb_strwidth of UTF-32BE string " . bin2hex($testString) . " was " .
      mb_strwidth($testString, 'UTF-32BE') . ", expected " . $testWidth);
}
echo "All good!\n";

?>
--EXPECT--
1: 68
2: 10
3: 21
4: 21
5: 21
mb_strwidth works as expected on all 'wide' characters
mb_strwidth works as expected on all 'narrow' characters
All good!
