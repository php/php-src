--TEST--
Bug #27675 (str_ireplace segfaults when shrinking string)
--FILE--
<?php
echo str_ireplace('/*<B>', '<B>', '/*<b> I am a comment</b>*/');
?>
--EXPECT--
<B> I am a comment</b>*/
