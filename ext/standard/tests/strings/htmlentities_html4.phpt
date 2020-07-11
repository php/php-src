--TEST--
htmlentities() conformance check (HTML 4)
--FILE--
<?php
function utf32_utf8($k) {
    if ($k < 0x80) {
        $retval = pack('C', $k);
    } else if ($k < 0x800) {
        $retval = pack('C2',
            0xc0 | ($k >> 6),
            0x80 | ($k & 0x3f));
    } else if ($k < 0x10000) {
        $retval = pack('C3',
            0xe0 | ($k >> 12),
            0x80 | (($k >> 6) & 0x3f),
            0x80 | ($k & 0x3f));
    } else if ($k < 0x200000) {
        $retval = pack('C4',
            0xf0 | ($k >> 18),
            0x80 | (($k >> 12) & 0x3f),
            0x80 | (($k >> 6) & 0x3f),
            0x80 | ($k & 0x3f));
    } else if ($k < 0x4000000) {
        $retval = pack('C5',
            0xf8 | ($k >> 24),
            0x80 | (($k >> 18) & 0x3f),
            0x80 | (($k >> 12) & 0x3f),
            0x80 | (($k >> 6) & 0x3f),
            0x80 | ($k & 0x3f));
    } else {
        $retval = pack('C6',
            0xfc | ($k >> 30),
            0x80 | (($k >> 24) & 0x3f),
            0x80 | (($k >> 18) & 0x3f),
            0x80 | (($k >> 12) & 0x3f),
            0x80 | (($k >> 6) & 0x3f),
            0x80 | ($k & 0x3f));
    }
    return $retval;
}

$table = get_html_translation_table(HTML_ENTITIES, ENT_QUOTES, 'UTF-8');

for ($i = 0; $i < 0x2710; $i++) {
    if ($i >= 0xd800 && $i < 0xe000)
        continue;
    $str = utf32_utf8($i);
    if (isset($table[$str])) {
        printf("%s\tU+%05X\n", $table[$str], $i);
        unset($table[$str]);
    }
}

if (!empty($table)) {
    echo "Not matched entities: ";
    var_dump($table);
}

?>
--EXPECT--
&quot;	U+00022
&amp;	U+00026
&#039;	U+00027
&lt;	U+0003C
&gt;	U+0003E
&nbsp;	U+000A0
&iexcl;	U+000A1
&cent;	U+000A2
&pound;	U+000A3
&curren;	U+000A4
&yen;	U+000A5
&brvbar;	U+000A6
&sect;	U+000A7
&uml;	U+000A8
&copy;	U+000A9
&ordf;	U+000AA
&laquo;	U+000AB
&not;	U+000AC
&shy;	U+000AD
&reg;	U+000AE
&macr;	U+000AF
&deg;	U+000B0
&plusmn;	U+000B1
&sup2;	U+000B2
&sup3;	U+000B3
&acute;	U+000B4
&micro;	U+000B5
&para;	U+000B6
&middot;	U+000B7
&cedil;	U+000B8
&sup1;	U+000B9
&ordm;	U+000BA
&raquo;	U+000BB
&frac14;	U+000BC
&frac12;	U+000BD
&frac34;	U+000BE
&iquest;	U+000BF
&Agrave;	U+000C0
&Aacute;	U+000C1
&Acirc;	U+000C2
&Atilde;	U+000C3
&Auml;	U+000C4
&Aring;	U+000C5
&AElig;	U+000C6
&Ccedil;	U+000C7
&Egrave;	U+000C8
&Eacute;	U+000C9
&Ecirc;	U+000CA
&Euml;	U+000CB
&Igrave;	U+000CC
&Iacute;	U+000CD
&Icirc;	U+000CE
&Iuml;	U+000CF
&ETH;	U+000D0
&Ntilde;	U+000D1
&Ograve;	U+000D2
&Oacute;	U+000D3
&Ocirc;	U+000D4
&Otilde;	U+000D5
&Ouml;	U+000D6
&times;	U+000D7
&Oslash;	U+000D8
&Ugrave;	U+000D9
&Uacute;	U+000DA
&Ucirc;	U+000DB
&Uuml;	U+000DC
&Yacute;	U+000DD
&THORN;	U+000DE
&szlig;	U+000DF
&agrave;	U+000E0
&aacute;	U+000E1
&acirc;	U+000E2
&atilde;	U+000E3
&auml;	U+000E4
&aring;	U+000E5
&aelig;	U+000E6
&ccedil;	U+000E7
&egrave;	U+000E8
&eacute;	U+000E9
&ecirc;	U+000EA
&euml;	U+000EB
&igrave;	U+000EC
&iacute;	U+000ED
&icirc;	U+000EE
&iuml;	U+000EF
&eth;	U+000F0
&ntilde;	U+000F1
&ograve;	U+000F2
&oacute;	U+000F3
&ocirc;	U+000F4
&otilde;	U+000F5
&ouml;	U+000F6
&divide;	U+000F7
&oslash;	U+000F8
&ugrave;	U+000F9
&uacute;	U+000FA
&ucirc;	U+000FB
&uuml;	U+000FC
&yacute;	U+000FD
&thorn;	U+000FE
&yuml;	U+000FF
&OElig;	U+00152
&oelig;	U+00153
&Scaron;	U+00160
&scaron;	U+00161
&Yuml;	U+00178
&fnof;	U+00192
&circ;	U+002C6
&tilde;	U+002DC
&Alpha;	U+00391
&Beta;	U+00392
&Gamma;	U+00393
&Delta;	U+00394
&Epsilon;	U+00395
&Zeta;	U+00396
&Eta;	U+00397
&Theta;	U+00398
&Iota;	U+00399
&Kappa;	U+0039A
&Lambda;	U+0039B
&Mu;	U+0039C
&Nu;	U+0039D
&Xi;	U+0039E
&Omicron;	U+0039F
&Pi;	U+003A0
&Rho;	U+003A1
&Sigma;	U+003A3
&Tau;	U+003A4
&Upsilon;	U+003A5
&Phi;	U+003A6
&Chi;	U+003A7
&Psi;	U+003A8
&Omega;	U+003A9
&alpha;	U+003B1
&beta;	U+003B2
&gamma;	U+003B3
&delta;	U+003B4
&epsilon;	U+003B5
&zeta;	U+003B6
&eta;	U+003B7
&theta;	U+003B8
&iota;	U+003B9
&kappa;	U+003BA
&lambda;	U+003BB
&mu;	U+003BC
&nu;	U+003BD
&xi;	U+003BE
&omicron;	U+003BF
&pi;	U+003C0
&rho;	U+003C1
&sigmaf;	U+003C2
&sigma;	U+003C3
&tau;	U+003C4
&upsilon;	U+003C5
&phi;	U+003C6
&chi;	U+003C7
&psi;	U+003C8
&omega;	U+003C9
&thetasym;	U+003D1
&upsih;	U+003D2
&piv;	U+003D6
&ensp;	U+02002
&emsp;	U+02003
&thinsp;	U+02009
&zwnj;	U+0200C
&zwj;	U+0200D
&lrm;	U+0200E
&rlm;	U+0200F
&ndash;	U+02013
&mdash;	U+02014
&lsquo;	U+02018
&rsquo;	U+02019
&sbquo;	U+0201A
&ldquo;	U+0201C
&rdquo;	U+0201D
&bdquo;	U+0201E
&dagger;	U+02020
&Dagger;	U+02021
&bull;	U+02022
&hellip;	U+02026
&permil;	U+02030
&prime;	U+02032
&Prime;	U+02033
&lsaquo;	U+02039
&rsaquo;	U+0203A
&oline;	U+0203E
&frasl;	U+02044
&euro;	U+020AC
&image;	U+02111
&weierp;	U+02118
&real;	U+0211C
&trade;	U+02122
&alefsym;	U+02135
&larr;	U+02190
&uarr;	U+02191
&rarr;	U+02192
&darr;	U+02193
&harr;	U+02194
&crarr;	U+021B5
&lArr;	U+021D0
&uArr;	U+021D1
&rArr;	U+021D2
&dArr;	U+021D3
&hArr;	U+021D4
&forall;	U+02200
&part;	U+02202
&exist;	U+02203
&empty;	U+02205
&nabla;	U+02207
&isin;	U+02208
&notin;	U+02209
&ni;	U+0220B
&prod;	U+0220F
&sum;	U+02211
&minus;	U+02212
&lowast;	U+02217
&radic;	U+0221A
&prop;	U+0221D
&infin;	U+0221E
&ang;	U+02220
&and;	U+02227
&or;	U+02228
&cap;	U+02229
&cup;	U+0222A
&int;	U+0222B
&there4;	U+02234
&sim;	U+0223C
&cong;	U+02245
&asymp;	U+02248
&ne;	U+02260
&equiv;	U+02261
&le;	U+02264
&ge;	U+02265
&sub;	U+02282
&sup;	U+02283
&nsub;	U+02284
&sube;	U+02286
&supe;	U+02287
&oplus;	U+02295
&otimes;	U+02297
&perp;	U+022A5
&sdot;	U+022C5
&lceil;	U+02308
&rceil;	U+02309
&lfloor;	U+0230A
&rfloor;	U+0230B
&lang;	U+02329
&rang;	U+0232A
&loz;	U+025CA
&spades;	U+02660
&clubs;	U+02663
&hearts;	U+02665
&diams;	U+02666
