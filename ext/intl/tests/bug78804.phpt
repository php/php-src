--TEST--
Bug #78804: Segmentation fault in Locale::filterMatches
--FILE--
<?php

if (Locale::filterMatches('en-US', 'und', true)) {
    echo 'Matches';
} else {
    echo 'Not matches';
}
?>
--EXPECT--
Not matches