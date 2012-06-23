--TEST--
RecursiveDirectoryIterator::getBasePath() - basic test
--CREDITS--
Pawel Krynicki <pawel [dot] krynicki [at] xsolve [dot] pl>
#testfest AmsterdamPHP 2012-06-23
--FILE--
<?php 
	$depth0 = md5('recursiveDirectoryIterator::getSubPath');
	$depth1 = md5('depth1');
	$depth2 = md5('depth2');
	$targetDir = __DIR__.DIRECTORY_SEPARATOR.$depth0.DIRECTORY_SEPARATOR.$depth1.DIRECTORY_SEPARATOR.$depth2;
	mkdir($targetDir, 0777, true);
	touch($targetDir.DIRECTORY_SEPARATOR.'getSubPath_test.tmp');
	$iterator = new RecursiveDirectoryIterator(__DIR__.DIRECTORY_SEPARATOR.$depth0);
	$it = new RecursiveIteratorIterator($iterator);

	while($it->valid()) 
	{
      echo $it->getSubPath()."\n";
	  $it->next();
	}
?>
--CLEAN--
<?php
	function rrmdir($dir) 
	{
	    foreach(glob($dir . '/*') as $file) 
	    {
          if(is_dir($file))
          {
            rrmdir($file);
          }
          else
          { 
            unlink($file);
          }
	    }

	    rmdir($dir);
	}

	$targetDir = __DIR__.DIRECTORY_SEPARATOR.md5('recursiveDirectoryIterator::getSubPath');
	rrmdir($targetDir);
?>

--EXPECT--
a0c967a6c2c34786e4802f59af9356f5/9925aabb545352472e4d77942627b507
a0c967a6c2c34786e4802f59af9356f5