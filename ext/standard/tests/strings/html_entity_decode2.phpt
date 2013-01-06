--TEST--
html_entity_decode: Handling of &apos;
--FILE--
<?php

echo "*** HTML 4.01 implicit (shouldn't decode) ***\n";
echo html_entity_decode("&apos;", ENT_QUOTES, "UTF-8"), "\n";

echo "*** HTML 4.01 (shouldn't decode) ***\n";
echo html_entity_decode("&apos;", ENT_QUOTES | ENT_HTML401, "UTF-8"), "\n";

echo "*** HTML 5 ***\n";
echo html_entity_decode("&apos;", ENT_QUOTES | ENT_HTML5, "UTF-8"), "\n";

echo "*** XHTML 1.0 ***\n";
echo html_entity_decode("&apos;", ENT_QUOTES | ENT_XHTML, "UTF-8"), "\n";

echo "*** XML 1.0 ***\n";
echo html_entity_decode("&apos;", ENT_QUOTES | ENT_XML1, "UTF-8"), "\n";

echo "Done.\n";
--EXPECT--
*** HTML 4.01 implicit (shouldn't decode) ***
&apos;
*** HTML 4.01 (shouldn't decode) ***
&apos;
*** HTML 5 ***
'
*** XHTML 1.0 ***
'
*** XML 1.0 ***
'
Done.