--TEST--
convert.quoted-printable-decode: lowercase hex digits decode correctly
--FILE--
<?php
foreach (['=cd', '=0a', '=da', '=CD', '=AB', '=ab'] as $in) {
    $fp = fopen('php://temp', 'r+');
    fwrite($fp, $in);
    rewind($fp);
    stream_filter_append($fp, 'convert.quoted-printable-decode', STREAM_FILTER_READ);
    echo $in, ' => ', bin2hex(stream_get_contents($fp)), "\n";
    fclose($fp);
}
?>
--EXPECT--
=cd => cd
=0a => 0a
=da => da
=CD => cd
=AB => ab
=ab => ab
