--TEST--
Bug #22538 (filtered stream doesn't update file pointer)
--FILE--
<?php
$fin = fopen(__FILE__, "r");
stream_filter_append(STDOUT, "string.rot13");
stream_copy_to_stream($fin, STDOUT);
fclose($fin);
?>
--EXPECT--
<?cuc
$sva = sbcra(__SVYR__, "e");
fgernz_svygre_nccraq(FGQBHG, "fgevat.ebg13");
fgernz_pbcl_gb_fgernz($sva, FGQBHG);
spybfr($sva);
?>
