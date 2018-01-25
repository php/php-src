--TEST--
locale_get_display_name() icu >= 53.1
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '53.1') < 0) die('skip for ICU >= 53.1'); ?>
--FILE--
<?php

/*
 * Try getting the display_name for different locales
 * with Procedural and Object methods.
 */

function ut_main()
{
    $res_str='';

	$disp_locales=array('en','fr','de');

    $locales = array(
        'sl_IT_nedis_KIRTI',
        'sl_IT_nedis-a-kirti-x-xyz',
        'sl_IT_rozaj',
        'sl_IT_NEDIS_ROJAZ_1901',
        'i-enochian',
        'zh-hakka',
        'zh-wuu',
        'i-tay',
        'sgn-BE-nl',
        'sgn-CH-de',
        'sl_IT_rozaj@currency=EUR',
        'uk-ua_CALIFORNIA@currency=;currency=GRN',
        'root',
        'uk@currency=EURO',
        'Hindi',
//Simple language subtag
        'de',
        'fr',
        'ja',
        'i-enochian', //(example of a grandfathered tag)
//Language subtag plus Script subtag:
        'zh-Hant',
        'zh-Hans',
        'sr-Cyrl',
        'sr-Latn',
//Language-Script-Region
        'zh-Hans-CN',
        'sr-Latn-CS',
//Language-Variant
        'sl-rozaj',
        'sl-nedis',
//Language-Region-Variant
        'de-CH-1901',
        'sl-IT-nedis',
//Language-Script-Region-Variant
        'sl-Latn-IT-nedis',
//Language-Region:
        'de-DE',
        'en-US',
        'es-419',
//Private use subtags:
        'de-CH-x-phonebk',
        'az-Arab-x-AZE-derbend',
//Extended language subtags
        'zh-min',
        'zh-min-nan-Hant-CN',
//Private use registry values
        'x-whatever',
        'qaa-Qaaa-QM-x-southern',
        'sr-Latn-QM',
        'sr-Qaaa-CS',
/*Tags that use extensions (examples ONLY: extensions MUST be defined
   by revision or update to this document or by RFC): */
        'en-US-u-islamCal',
        'zh-CN-a-myExt-x-private',
        'en-a-myExt-b-another',
//Some Invalid Tags:
        'de-419-DE',
        'a-DE',
        'ar-a-aaa-b-bbb-a-ccc'
    );


    $res_str = '';

   	foreach( $locales as $locale )
    {
       	$res_str .= "locale='$locale'\n";
   		foreach( $disp_locales as $disp_locale )
    	{
        	$scr = ut_loc_get_display_name( $locale ,$disp_locale );
		$scr = str_replace(array('(', ')'), '#', $scr);
        	$res_str .= "disp_locale=$disp_locale :  display_name=$scr";
        	$res_str .= "\n";
		}
        $res_str .= "-----------------\n";
    }

    return $res_str;

}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECTREGEX--
locale='sl_IT_nedis_KIRTI'
disp_locale=en :  display_name=Slovenian #Italy, NEDIS_KIRTI#
disp_locale=fr :  display_name=slovène #Italie, NEDIS_KIRTI#
disp_locale=de :  display_name=Slowenisch #Italien, NEDIS_KIRTI#
-----------------
locale='sl_IT_nedis-a-kirti-x-xyz'
disp_locale=en :  display_name=Slovenian #Italy, NEDIS_A_KIRTI_X_XYZ#
disp_locale=fr :  display_name=slovène #Italie, NEDIS_A_KIRTI_X_XYZ#
disp_locale=de :  display_name=Slowenisch #Italien, NEDIS_A_KIRTI_X_XYZ#
-----------------
locale='sl_IT_rozaj'
disp_locale=en :  display_name=Slovenian #Italy, Resian#
disp_locale=fr :  display_name=slovène #Italie, dialecte de Resia#
disp_locale=de :  display_name=Slowenisch #Italien, (ROZAJ|Resianisch)#
-----------------
locale='sl_IT_NEDIS_ROJAZ_1901'
disp_locale=en :  display_name=Slovenian #Italy, NEDIS_ROJAZ_1901#
disp_locale=fr :  display_name=slovène #Italie, NEDIS_ROJAZ_1901#
disp_locale=de :  display_name=Slowenisch #Italien, NEDIS_ROJAZ_1901#
-----------------
locale='i-enochian'
disp_locale=en :  display_name=i-enochian #Private-Use=i-enochian#
disp_locale=fr :  display_name=i-enochian #Usage privé=i-enochian#
disp_locale=de :  display_name=i-enochian #Privatnutzung=i-enochian#
-----------------
locale='zh-hakka'
disp_locale=en :  display_name=Chinese( #HAKKA#)?
disp_locale=fr :  display_name=chinois( #HAKKA#)?
disp_locale=de :  display_name=Chinesisch( #HAKKA#)?
-----------------
locale='zh-wuu'
disp_locale=en :  display_name=Chinese #WUU#
disp_locale=fr :  display_name=chinois #WUU#
disp_locale=de :  display_name=Chinesisch #WUU#
-----------------
locale='i-tay'
disp_locale=en :  display_name=i-tay
disp_locale=fr :  display_name=i-tay
disp_locale=de :  display_name=i-tay
-----------------
locale='sgn-BE-nl'
disp_locale=en :  display_name=sgn #Belgium, NL#
disp_locale=fr :  display_name=sgn #Belgique, NL#
disp_locale=de :  display_name=sgn #Belgien, NL#
-----------------
locale='sgn-CH-de'
disp_locale=en :  display_name=sgn #Switzerland, DE#
disp_locale=fr :  display_name=sgn #Suisse, DE#
disp_locale=de :  display_name=sgn #Schweiz, DE#
-----------------
locale='sl_IT_rozaj@currency=EUR'
disp_locale=en :  display_name=Slovenian #Italy, Resian, [Cc]urrency=Euro#
disp_locale=fr :  display_name=slovène #Italie, dialecte de Resia, [Dd]evise=euro#
disp_locale=de :  display_name=Slowenisch #Italien, (ROZAJ|Resianisch), Währung=Euro#
-----------------
locale='uk-ua_CALIFORNIA@currency=;currency=GRN'
disp_locale=en :  display_name=
disp_locale=fr :  display_name=
disp_locale=de :  display_name=
-----------------
locale='root'
disp_locale=en :  display_name=Root
disp_locale=fr :  display_name=racine
disp_locale=de :  display_name=[Rr]oot
-----------------
locale='uk@currency=EURO'
disp_locale=en :  display_name=Ukrainian #[Cc]urrency=EURO#
disp_locale=fr :  display_name=ukrainien #[Dd]evise=EURO#
disp_locale=de :  display_name=Ukrainisch #Währung=EURO#
-----------------
locale='Hindi'
disp_locale=en :  display_name=hindi
disp_locale=fr :  display_name=hindi
disp_locale=de :  display_name=hindi
-----------------
locale='de'
disp_locale=en :  display_name=German
disp_locale=fr :  display_name=allemand
disp_locale=de :  display_name=Deutsch
-----------------
locale='fr'
disp_locale=en :  display_name=French
disp_locale=fr :  display_name=français
disp_locale=de :  display_name=Französisch
-----------------
locale='ja'
disp_locale=en :  display_name=Japanese
disp_locale=fr :  display_name=japonais
disp_locale=de :  display_name=Japanisch
-----------------
locale='i-enochian'
disp_locale=en :  display_name=i-enochian #Private-Use=i-enochian#
disp_locale=fr :  display_name=i-enochian #Usage privé=i-enochian#
disp_locale=de :  display_name=i-enochian #Privatnutzung=i-enochian#
-----------------
locale='zh-Hant'
disp_locale=en :  display_name=Chinese #Traditional#
disp_locale=fr :  display_name=chinois #traditionnel#
disp_locale=de :  display_name=Chinesisch #Traditionell#
-----------------
locale='zh-Hans'
disp_locale=en :  display_name=Chinese #Simplified#
disp_locale=fr :  display_name=chinois #simplifié#
disp_locale=de :  display_name=Chinesisch #Vereinfacht#
-----------------
locale='sr-Cyrl'
disp_locale=en :  display_name=Serbian #Cyrillic#
disp_locale=fr :  display_name=serbe #cyrillique#
disp_locale=de :  display_name=Serbisch #Kyrillisch#
-----------------
locale='sr-Latn'
disp_locale=en :  display_name=Serbian #Latin#
disp_locale=fr :  display_name=serbe #latin#
disp_locale=de :  display_name=Serbisch #Lateinisch#
-----------------
locale='zh-Hans-CN'
disp_locale=en :  display_name=Chinese #Simplified, China#
disp_locale=fr :  display_name=chinois #simplifié, Chine#
disp_locale=de :  display_name=Chinesisch #Vereinfacht, China#
-----------------
locale='sr-Latn-CS'
disp_locale=en :  display_name=Serbian #Latin, Serbia#
disp_locale=fr :  display_name=serbe #latin, Serbie#
disp_locale=de :  display_name=Serbisch #Lateinisch, Serbien#
-----------------
locale='sl-rozaj'
disp_locale=en :  display_name=Slovenian #Resian#
disp_locale=fr :  display_name=slovène #dialecte de Resia#
disp_locale=de :  display_name=Slowenisch( #(ROZAJ|Resianisch)#)?
-----------------
locale='sl-nedis'
disp_locale=en :  display_name=Slovenian #Natisone dialect#
disp_locale=fr :  display_name=slovène #dialecte de Natisone#
disp_locale=de :  display_name=Slowenisch #Natisone-Dialekt#
-----------------
locale='de-CH-1901'
disp_locale=en :  display_name=German #Switzerland, Traditional German orthography#
disp_locale=fr :  display_name=allemand #Suisse, orthographe allemande traditionnelle#
disp_locale=de :  display_name=Deutsch #Schweiz, (1901|[aA]lte deutsche Rechtschreibung)#
-----------------
locale='sl-IT-nedis'
disp_locale=en :  display_name=Slovenian #Italy, Natisone dialect#
disp_locale=fr :  display_name=slovène #Italie, dialecte de Natisone#
disp_locale=de :  display_name=Slowenisch #Italien, (NEDIS|Natisone-Dialekt)#
-----------------
locale='sl-Latn-IT-nedis'
disp_locale=en :  display_name=Slovenian #Latin, Italy, Natisone dialect#
disp_locale=fr :  display_name=slovène #latin, Italie, dialecte de Natisone#
disp_locale=de :  display_name=Slowenisch #Lateinisch, Italien, (NEDIS|Natisone-Dialekt)#
-----------------
locale='de-DE'
disp_locale=en :  display_name=German #Germany#
disp_locale=fr :  display_name=allemand #Allemagne#
disp_locale=de :  display_name=Deutsch #Deutschland#
-----------------
locale='en-US'
disp_locale=en :  display_name=English #United States#
disp_locale=fr :  display_name=anglais #États-Unis#
disp_locale=de :  display_name=Englisch #Vereinigte Staaten#
-----------------
locale='es-419'
disp_locale=en :  display_name=Spanish #Latin America#
disp_locale=fr :  display_name=espagnol #Amérique latine#
disp_locale=de :  display_name=Spanisch #Lateinamerika#
-----------------
locale='de-CH-x-phonebk'
disp_locale=en :  display_name=German #Switzerland, Private-Use=phonebk#
disp_locale=fr :  display_name=allemand #Suisse, Usage privé=phonebk#
disp_locale=de :  display_name=Deutsch #Schweiz, Privatnutzung=phonebk#
-----------------
locale='az-Arab-x-AZE-derbend'
disp_locale=en :  display_name=Azerbaijani #Arabic, Private-Use=aze-derbend#
disp_locale=fr :  display_name=azéri #arabe, Usage privé=aze-derbend#
disp_locale=de :  display_name=Aserbaidschanisch #Arabisch, Privatnutzung=aze-derbend#
-----------------
locale='zh-min'
disp_locale=en :  display_name=Chinese #MIN#
disp_locale=fr :  display_name=chinois #MIN#
disp_locale=de :  display_name=Chinesisch #MIN#
-----------------
locale='zh-min-nan-Hant-CN'
disp_locale=en :  display_name=Chinese #MIN, NAN_HANT_CN#
disp_locale=fr :  display_name=chinois #MIN, NAN_HANT_CN#
disp_locale=de :  display_name=Chinesisch #MIN, NAN_HANT_CN#
-----------------
locale='x-whatever'
disp_locale=en :  display_name=x-whatever #Private-Use=whatever#
disp_locale=fr :  display_name=x-whatever #Usage privé=whatever#
disp_locale=de :  display_name=x-whatever #Privatnutzung=whatever#
-----------------
locale='qaa-Qaaa-QM-x-southern'
disp_locale=en :  display_name=qaa #Qaaa, QM, Private-Use=southern#
disp_locale=fr :  display_name=qaa #Qaaa, QM, Usage privé=southern#
disp_locale=de :  display_name=qaa #Qaaa, QM, Privatnutzung=southern#
-----------------
locale='sr-Latn-QM'
disp_locale=en :  display_name=Serbian #Latin, QM#
disp_locale=fr :  display_name=serbe #latin, QM#
disp_locale=de :  display_name=Serbisch #Lateinisch, QM#
-----------------
locale='sr-Qaaa-CS'
disp_locale=en :  display_name=Serbian #Qaaa, Serbia#
disp_locale=fr :  display_name=serbe #Qaaa, Serbie#
disp_locale=de :  display_name=Serbisch #Qaaa, Serbien#
-----------------
locale='en-US-u-islamCal'
disp_locale=en :  display_name=English #United States, attribute=islamcal#
disp_locale=fr :  display_name=anglais #États-Unis, attribute=islamcal#
disp_locale=de :  display_name=Englisch #Vereinigte Staaten, attribute=islamcal#
-----------------
locale='zh-CN-a-myExt-x-private'
disp_locale=en :  display_name=Chinese #China, a=myext, Private-Use=private#
disp_locale=fr :  display_name=chinois #Chine, a=myext, Usage privé=private#
disp_locale=de :  display_name=Chinesisch #China, a=myext, Privatnutzung=private#
-----------------
locale='en-a-myExt-b-another'
disp_locale=en :  display_name=English #a=myext, b=another#
disp_locale=fr :  display_name=anglais #a=myext, b=another#
disp_locale=de :  display_name=Englisch #a=myext, b=another#
-----------------
locale='de-419-DE'
disp_locale=en :  display_name=German #Latin America, DE#
disp_locale=fr :  display_name=allemand #Amérique latine, DE#
disp_locale=de :  display_name=Deutsch #Lateinamerika, DE#
-----------------
locale='a-DE'
disp_locale=en :  display_name=a #Germany#
disp_locale=fr :  display_name=a #Allemagne#
disp_locale=de :  display_name=a #Deutschland#
-----------------
locale='ar-a-aaa-b-bbb-a-ccc'
disp_locale=en :  display_name=Arabic #a=aaa, b=bbb#
disp_locale=fr :  display_name=arabe #a=aaa, b=bbb#
disp_locale=de :  display_name=Arabisch #a=aaa, b=bbb#
-----------------
