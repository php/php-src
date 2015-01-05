--TEST--
DirectoryIterator::getBasename() - Pass unexpected array
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php
   $targetDir = __DIR__.DIRECTORY_SEPARATOR.md5('directoryIterator::getbasename2');
   mkdir($targetDir);
   touch($targetDir.DIRECTORY_SEPARATOR.'getBasename_test.txt');
   $dir = new DirectoryIterator($targetDir.DIRECTORY_SEPARATOR);
   while(!$dir->isFile()) {
      $dir->next();
   }
   echo $dir->getBasename(array());
?>
--CLEAN--
<?php
   $targetDir = __DIR__.DIRECTORY_SEPARATOR.md5('directoryIterator::getbasename2');
   unlink($targetDir.DIRECTORY_SEPARATOR.'getBasename_test.txt');
   rmdir($targetDir);
?>
--EXPECTF--
Warning: DirectoryIterator::getBasename() expects parameter 1 to be %binary_string_optional%, array given in %s on line %d
