--TEST--
DirectoryIterator::getBasename() - Basic Test
--CREDITS--
PHPNW Testfest 2009 - Adrian Hardy
--FILE--
<?php
   $targetDir = __DIR__.DIRECTORY_SEPARATOR.md5('directoryIterator::getbasename1');
   mkdir($targetDir);
   touch($targetDir.DIRECTORY_SEPARATOR.'getBasename_test.txt');
   $dir = new DirectoryIterator($targetDir.DIRECTORY_SEPARATOR);
   while(!$dir->isFile()) {
      $dir->next();
   }
   echo $dir->getBasename('.txt');
?>
--CLEAN--
<?php
   $targetDir = __DIR__.DIRECTORY_SEPARATOR.md5('directoryIterator::getbasename1');
   unlink($targetDir.DIRECTORY_SEPARATOR.'getBasename_test.txt');
   rmdir($targetDir);
?>
--EXPECT--
getBasename_test
