<?php

$parser = new PEAR_Common;
$parser->setErrorHandling(PEAR_ERROR_DIE, "pear info: %s\n");
$info = $parser->infoFromTgzFile($cmdargs[0]);
unset($info['filelist']);
present_array($info);

?>