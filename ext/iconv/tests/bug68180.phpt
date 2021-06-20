--TEST--
Bug #68180 (iconv_mime_decode can return extra characters in a header)
--EXTENSIONS--
iconv
--FILE--
<?php
$original = "=?UTF-8?Q?=E3=80=8E=E3=80=90=E5=A4=96=E8=B3=87=E7=B3=BB=E6=88=A6=E7=95=A5=E3=82=B3=E3=83=B3=E3=82=B5=E3=83=AB=E3=81=8C=E9=9B=86=E7=B5=90=E3=80=91=E3=83=88=E3=83=83=E3=83=97=E3=82=B3=E3=83=B3=E3=82=B5=E3=83=AB=E3=82=BF=E3=83=B3=E3=83=88=E3=81=A8=E8=A9=B1=E3=81=9B=E3=82=8B=E3=82=B3=E3=83=B3=E3=82=B5=E3=83=AB=E6=A5=AD=E7=95=8C=E7=A0=94=E7=A9=B6=E3=82=BB=E3=83=9F=E3=83=8A=E3=83=BC=E3=80=8F=E3=81=B8=E3=81=AE=E3=82=A8=E3=83=B3=E3=83=88=E3=83=AA=E3=83=BC=E3=81=82=E3=82=8A=E3=81=8C=E3=81=A8=E3=81=86=E3=81=94=E3=81=96=E3=81=84=E3=81=BE=E3=81=97=E3=81=9F=E3=80=82?=";
$decoded = iconv_mime_decode($original, ICONV_MIME_DECODE_STRICT, 'utf-8');
var_dump($decoded);
?>
--EXPECT--
string(183) "『【外資系戦略コンサルが集結】トップコンサルタントと話せるコンサル業界研究セミナー』へのエントリーありがとうございました。"
