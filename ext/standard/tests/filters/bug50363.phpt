--TEST--
Bug #50363 (Invalid parsing in convert.quoted-printable-decode filter)
--FILE--
<?php

$foo = "Sauvegarder=C3=A9ussi(e) n=C3=A3o N=C3=83O\n";
$foo .= "Sauvegarder=c3=a9ussi(e) n=c3=a3o N=c3=83O\n"; // Does not work!
$b = fopen('php://temp', 'w+');
stream_filter_append($b, 'convert.quoted-printable-decode', STREAM_FILTER_WRITE);
fwrite($b, $foo);
rewind($b);
fpassthru($b);

?>
--EXPECT--
Sauvegarderéussi(e) não NÃO
Sauvegarderéussi(e) não NÃO
