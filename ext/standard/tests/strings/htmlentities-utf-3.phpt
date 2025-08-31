--TEST--
Test get_next_char(), used by htmlentities()/htmlspecialchars(): validity of UTF-8 sequences
--FILE--
<?php

/* conformance to Unicode 5.2, section 3.9, D92 */

$val_ranges = array(
    array(array(0x00, 0x7F)),
    array(array(0xC2, 0xDF), array(0x80, 0xBF)),
    array(array(0xE0, 0xE0), array(0xA0, 0xBF), array(0x80, 0xBF)),
    array(array(0xE1, 0xEC), array(0x80, 0xBF), array(0x80, 0xBF)),
    array(array(0xED, 0xED), array(0x80, 0x9F), array(0x80, 0xBF)),
    array(array(0xEE, 0xEF), array(0x80, 0xBF), array(0x80, 0xBF)),
    array(array(0xF0, 0xF0), array(0x90, 0xBF), array(0x80, 0xBF), array(0x80, 0xBF)),
    array(array(0xF1, 0xF3), array(0x80, 0xBF), array(0x80, 0xBF), array(0x80, 0xBF)),
    array(array(0xF4, 0xF4), array(0x80, 0x8F), array(0x80, 0xBF), array(0x80, 0xBF)),
);

function is_valid($seq) {
    global $val_ranges;
    $b = ord($seq[0]);
    foreach ($val_ranges as $l) {
        if ($b >= $l[0][0] && $b <= $l[0][1]) {
            if (count($l) != strlen($seq)) {
                return false;
            }
            for ($n = 1; $n < strlen($seq); $n++) {
                if (ord($seq[$n]) < $l[$n][0] || ord($seq[$n]) > $l[$n][1]) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

function concordance($s) {
    $vhe = strlen(htmlspecialchars($s, ENT_QUOTES, "UTF-8")) > 0;
    $v = is_valid($s);
    return ($vhe === $v);
}

for ($b1 = 0xC0; $b1 < 0xE0; $b1++) {
    for ($b2 = 0x80; $b2 < 0xBF; $b2++) {
        $s = chr($b1).chr($b2);
        if (!concordance($s))
            echo "Discordance for ".bin2hex($s),"\n";
    }
}


for ($b1 = 0xE0; $b1 < 0xEF; $b1++) {
    for ($b2 = 0x80; $b2 < 0xBF; $b2++) {
        $s = chr($b1).chr($b2)."\x80";
        if (!concordance($s))
            echo "Discordance for ".bin2hex($s),"\n";
        $s = chr($b1).chr($b2)."\xBF";
        if (!concordance($s))
            echo "Discordance for ".bin2hex($s),"\n";
    }
}

for ($b1 = 0xF0; $b1 < 0xFF; $b1++) {
    for ($b2 = 0x80; $b2 < 0xBF; $b2++) {
        $s = chr($b1).chr($b2)."\x80\x80";
        if (!concordance($s))
            echo "Discordance for ".bin2hex($s),"\n";
        $s = chr($b1).chr($b2)."\xBF\x80";
        if (!concordance($s))
            echo "Discordance for ".bin2hex($s),"\n";
        $s = chr($b1).chr($b2)."\x80\xBF";
        if (!concordance($s))
            echo "Discordance for ".bin2hex($s),"\n";
        $s = chr($b1).chr($b2)."\xBF\xBF";
        if (!concordance($s))
            echo "Discordance for ".bin2hex($s),"\n";
    }
}
echo "Done.\n";
?>
--EXPECT--
Done.
