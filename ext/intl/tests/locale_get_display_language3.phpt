--TEST--
locale_get_display_language() for grandfathered and redundant tags
--EXTENSIONS--
intl
--FILE--
<?php

function ut_main()
{
    $res_str = '';
    $disp_locales = ['en', 'fr', 'de'];

    // This array includes all grandfathered tags, as well as any redundant tags
    // that have a Preferred-Value.
    // See: https://www.iana.org/assignments/language-subtag-registry/
    $locales = [
        'art-lojban',
        'cel-gaulish',
        'en-GB-oed',
        'i-ami',
        'i-bnn',
        'i-default',
        'i-enochian',
        'i-hak',
        'i-klingon',
        'i-lux',
        'i-mingo',
        'i-navajo',
        'i-pwn',
        'i-tao',
        'i-tay',
        'i-tsu',
        'no-bok',
        'no-nyn',
        'sgn-BE-FR',
        'sgn-BE-NL',
        'sgn-BR',
        'sgn-CH-DE',
        'sgn-CO',
        'sgn-DE',
        'sgn-DK',
        'sgn-ES',
        'sgn-FR',
        'sgn-GB',
        'sgn-GR',
        'sgn-IE',
        'sgn-IT',
        'sgn-JP',
        'sgn-MX',
        'sgn-NI',
        'sgn-NL',
        'sgn-NO',
        'sgn-PT',
        'sgn-SE',
        'sgn-US',
        'sgn-ZA',
        'zh-cmn',
        'zh-cmn-Hans',
        'zh-cmn-Hant',
        'zh-gan',
        'zh-guoyu',
        'zh-hakka',
        'zh-min',
        'zh-min-nan',
        'zh-wuu',
        'zh-xiang',
    ];

    foreach ($locales as $locale) {
        $res_str .= "locale='$locale'\n";

        foreach ($disp_locales as $disp_locale) {
            $scr = ut_loc_get_display_language($locale, $disp_locale);
            $res_str .= "disp_locale=$disp_locale :  display_language=$scr";
            $res_str .= "\n";
        }

        $res_str .= "-----------------\n";
    }

    return $res_str;
}

include_once 'ut_common.inc';
ut_run();

?>
--EXPECTREGEX--
locale='art-lojban'
disp_locale=en :  display_language=Lojban
disp_locale=fr :  display_language=lojban
disp_locale=de :  display_language=Lojban
-----------------
locale='cel-gaulish'
disp_locale=en :  display_language=cel
disp_locale=fr :  display_language=cel
disp_locale=de :  display_language=cel
-----------------
locale='en-GB-oed'
disp_locale=en :  display_language=English
disp_locale=fr :  display_language=anglais
disp_locale=de :  display_language=Englisch
-----------------
locale='i-ami'
disp_locale=en :  display_language=ami
disp_locale=fr :  display_language=ami
disp_locale=de :  display_language=ami
-----------------
locale='i-bnn'
disp_locale=en :  display_language=bnn
disp_locale=fr :  display_language=bnn
disp_locale=de :  display_language=bnn
-----------------
locale='i-default'
disp_locale=en :  display_language=i-default
disp_locale=fr :  display_language=i-default
disp_locale=de :  display_language=i-default
-----------------
locale='i-enochian'
disp_locale=en :  display_language=i-enochian
disp_locale=fr :  display_language=i-enochian
disp_locale=de :  display_language=i-enochian
-----------------
locale='i-hak'
disp_locale=en :  display_language=Hakka Chinese
disp_locale=fr :  display_language=hakka
disp_locale=de :  display_language=Hakka
-----------------
locale='i-klingon'
disp_locale=en :  display_language=Klingon
disp_locale=fr :  display_language=klingon
disp_locale=de :  display_language=Klingonisch
-----------------
locale='i-lux'
disp_locale=en :  display_language=Luxembourgish
disp_locale=fr :  display_language=luxembourgeois
disp_locale=de :  display_language=Luxemburgisch
-----------------
locale='i-mingo'
disp_locale=en :  display_language=i-mingo
disp_locale=fr :  display_language=i-mingo
disp_locale=de :  display_language=i-mingo
-----------------
locale='i-navajo'
disp_locale=en :  display_language=Navajo
disp_locale=fr :  display_language=(navajo|navaho)
disp_locale=de :  display_language=Navajo
-----------------
locale='i-pwn'
disp_locale=en :  display_language=pwn
disp_locale=fr :  display_language=pwn
disp_locale=de :  display_language=pwn
-----------------
locale='i-tao'
disp_locale=en :  display_language=tao
disp_locale=fr :  display_language=tao
disp_locale=de :  display_language=tao
-----------------
locale='i-tay'
disp_locale=en :  display_language=tay
disp_locale=fr :  display_language=tay
disp_locale=de :  display_language=tay
-----------------
locale='i-tsu'
disp_locale=en :  display_language=tsu
disp_locale=fr :  display_language=tsu
disp_locale=de :  display_language=tsu
-----------------
locale='no-bok'
disp_locale=en :  display_language=Norwegian Bokmål
disp_locale=fr :  display_language=norvégien bokmål
disp_locale=de :  display_language=Norwegisch \(?Bokmål\)?
-----------------
locale='no-nyn'
disp_locale=en :  display_language=Norwegian Nynorsk
disp_locale=fr :  display_language=norvégien nynorsk
disp_locale=de :  display_language=Norwegisch \(?Nynorsk\)?
-----------------
locale='sgn-BE-FR'
disp_locale=en :  display_language=sfb
disp_locale=fr :  display_language=sfb
disp_locale=de :  display_language=sfb
-----------------
locale='sgn-BE-NL'
disp_locale=en :  display_language=vgt
disp_locale=fr :  display_language=vgt
disp_locale=de :  display_language=vgt
-----------------
locale='sgn-BR'
disp_locale=en :  display_language=bzs
disp_locale=fr :  display_language=bzs
disp_locale=de :  display_language=bzs
-----------------
locale='sgn-CH-DE'
disp_locale=en :  display_language=sgg
disp_locale=fr :  display_language=sgg
disp_locale=de :  display_language=sgg
-----------------
locale='sgn-CO'
disp_locale=en :  display_language=csn
disp_locale=fr :  display_language=csn
disp_locale=de :  display_language=csn
-----------------
locale='sgn-DE'
disp_locale=en :  display_language=gsg
disp_locale=fr :  display_language=gsg
disp_locale=de :  display_language=gsg
-----------------
locale='sgn-DK'
disp_locale=en :  display_language=dsl
disp_locale=fr :  display_language=dsl
disp_locale=de :  display_language=dsl
-----------------
locale='sgn-ES'
disp_locale=en :  display_language=ssp
disp_locale=fr :  display_language=ssp
disp_locale=de :  display_language=ssp
-----------------
locale='sgn-FR'
disp_locale=en :  display_language=fsl
disp_locale=fr :  display_language=fsl
disp_locale=de :  display_language=fsl
-----------------
locale='sgn-GB'
disp_locale=en :  display_language=bfi
disp_locale=fr :  display_language=bfi
disp_locale=de :  display_language=bfi
-----------------
locale='sgn-GR'
disp_locale=en :  display_language=gss
disp_locale=fr :  display_language=gss
disp_locale=de :  display_language=gss
-----------------
locale='sgn-IE'
disp_locale=en :  display_language=isg
disp_locale=fr :  display_language=isg
disp_locale=de :  display_language=isg
-----------------
locale='sgn-IT'
disp_locale=en :  display_language=ise
disp_locale=fr :  display_language=ise
disp_locale=de :  display_language=ise
-----------------
locale='sgn-JP'
disp_locale=en :  display_language=jsl
disp_locale=fr :  display_language=jsl
disp_locale=de :  display_language=jsl
-----------------
locale='sgn-MX'
disp_locale=en :  display_language=mfs
disp_locale=fr :  display_language=mfs
disp_locale=de :  display_language=mfs
-----------------
locale='sgn-NI'
disp_locale=en :  display_language=ncs
disp_locale=fr :  display_language=ncs
disp_locale=de :  display_language=ncs
-----------------
locale='sgn-NL'
disp_locale=en :  display_language=dse
disp_locale=fr :  display_language=dse
disp_locale=de :  display_language=dse
-----------------
locale='sgn-NO'
disp_locale=en :  display_language=nsl
disp_locale=fr :  display_language=nsl
disp_locale=de :  display_language=nsl
-----------------
locale='sgn-PT'
disp_locale=en :  display_language=psr
disp_locale=fr :  display_language=psr
disp_locale=de :  display_language=psr
-----------------
locale='sgn-SE'
disp_locale=en :  display_language=swl
disp_locale=fr :  display_language=swl
disp_locale=de :  display_language=swl
-----------------
locale='sgn-US'
disp_locale=en :  display_language=American Sign Language
disp_locale=fr :  display_language=langue des signes américaine
disp_locale=de :  display_language=Amerikanische Gebärdensprache
-----------------
locale='sgn-ZA'
disp_locale=en :  display_language=sfs
disp_locale=fr :  display_language=sfs
disp_locale=de :  display_language=sfs
-----------------
locale='zh-cmn'
disp_locale=en :  display_language=(cmn|Chinese)
disp_locale=fr :  display_language=(cmn|chinois)
disp_locale=de :  display_language=(cmn|Chinesisch)
-----------------
locale='zh-cmn-Hans'
disp_locale=en :  display_language=(cmn|Chinese)
disp_locale=fr :  display_language=(cmn|chinois)
disp_locale=de :  display_language=(cmn|Chinesisch)
-----------------
locale='zh-cmn-Hant'
disp_locale=en :  display_language=(cmn|Chinese)
disp_locale=fr :  display_language=(cmn|chinois)
disp_locale=de :  display_language=(cmn|Chinesisch)
-----------------
locale='zh-gan'
disp_locale=en :  display_language=Gan Chinese
disp_locale=fr :  display_language=gan
disp_locale=de :  display_language=Gan
-----------------
locale='zh-guoyu'
disp_locale=en :  display_language=(cmn|Chinese)
disp_locale=fr :  display_language=(cmn|chinois)
disp_locale=de :  display_language=(cmn|Chinesisch)
-----------------
locale='zh-hakka'
disp_locale=en :  display_language=Hakka Chinese
disp_locale=fr :  display_language=hakka
disp_locale=de :  display_language=Hakka
-----------------
locale='zh-min'
disp_locale=en :  display_language=Chinese
disp_locale=fr :  display_language=chinois
disp_locale=de :  display_language=Chinesisch
-----------------
locale='zh-min-nan'
disp_locale=en :  display_language=Min Nan Chinese
disp_locale=fr :  display_language=minnan
disp_locale=de :  display_language=Min Nan
-----------------
locale='zh-wuu'
disp_locale=en :  display_language=Wu Chinese
disp_locale=fr :  display_language=wu
disp_locale=de :  display_language=Wu
-----------------
locale='zh-xiang'
disp_locale=en :  display_language=Xiang Chinese
disp_locale=fr :  display_language=xiang
disp_locale=de :  display_language=Xiang
-----------------
