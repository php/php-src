--TEST--
Test fopen() function : variation: file uri, no use include path
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Run only on Windows");
?>
--FILE--
<?php
echo "*** Testing fopen() : variation ***\n";

// fopen with interesting windows paths.
$testDir = 'fopen14.tmpDir';
$absTestDir = getcwd().'/'.$testDir;
$file = "fopen_variation14.tmp";
$unixifiedDir = '/'.substr(str_replace('\\','/',$absTestDir),3);
$absFile = $absTestDir.'/'.$file;

mkdir($testDir);

$files = array("file://$testDir\\$file",
               "file://$testDir/$file",
               "file://./$testDir/$file",
               "file://.\\$testDir\\$file",
               "file://$absTestDir/$file",
               "file://$absTestDir\\$file",
               "file://$unixifiedDir/$file"
);

runtest($files);

chdir($testDir);
$files = array("file://../$testDir/$file",
               "file://..\\$testDir\\$file",
               "file://$absTestDir/$file",
               "file://$absTestDir\\$file",
               "file://$unixifiedDir/$file"
);
runtest($files);
chdir("..");
rmdir($testDir);

function runtest($fileURIs) {
   global $absFile;
   $iteration = 0;
   foreach($fileURIs as $fileURI) {
      echo "--- READ: $fileURI ---\n";

      $readData = "read:$iteration";
      $writeData = "write:$iteration";

      // create the file and test read
      $h = fopen($absFile, 'w');
      fwrite($h, $readData);
      fclose($h);

      $h = fopen($fileURI, 'r');
      if ($h !== false) {
         if (fread($h, 4096) != $readData) {
            echo "contents not correct\n";
         }
         else {
            echo "test passed\n";
         }
         fclose($h);
      }
      unlink($absFile);

      echo "--- WRITE: $fileURI ---\n";
      // create the file to test write
      $h = fopen($fileURI, 'w');
      if ($h !== false) {
          fwrite($h, $writeData);
          fclose($h);

          $h = fopen($absFile, 'r');
          if ($h !== false) {
             if (fread($h, 4096) != $writeData) {
                echo "contents not correct\n";
             }
             else {
                echo "test passed\n";
             }
             fclose($h);
          }
          unlink($absFile);
       }
   }
}


?>
--EXPECTF--
*** Testing fopen() : variation ***
--- READ: file://fopen14.tmpDir\fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://fopen14.tmpDir\fopen_variation14.tmp in %s on line %d

Warning: fopen(file://fopen14.tmpDir\fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- WRITE: file://fopen14.tmpDir\fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://fopen14.tmpDir\fopen_variation14.tmp in %s on line %d

Warning: fopen(file://fopen14.tmpDir\fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- READ: file://fopen14.tmpDir/fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://fopen14.tmpDir/fopen_variation14.tmp in %s on line %d

Warning: fopen(file://fopen14.tmpDir/fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- WRITE: file://fopen14.tmpDir/fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://fopen14.tmpDir/fopen_variation14.tmp in %s on line %d

Warning: fopen(file://fopen14.tmpDir/fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- READ: file://./fopen14.tmpDir/fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://./fopen14.tmpDir/fopen_variation14.tmp in %s on line %d

Warning: fopen(file://./fopen14.tmpDir/fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- WRITE: file://./fopen14.tmpDir/fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://./fopen14.tmpDir/fopen_variation14.tmp in %s on line %d

Warning: fopen(file://./fopen14.tmpDir/fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- READ: file://.\fopen14.tmpDir\fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://.\fopen14.tmpDir\fopen_variation14.tmp in %s on line %d

Warning: fopen(file://.\fopen14.tmpDir\fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- WRITE: file://.\fopen14.tmpDir\fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://.\fopen14.tmpDir\fopen_variation14.tmp in %s on line %d

Warning: fopen(file://.\fopen14.tmpDir\fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- READ: file://%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
--- WRITE: file://%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
--- READ: file://%s/fopen14.tmpDir\fopen_variation14.tmp ---
test passed
--- WRITE: file://%s/fopen14.tmpDir\fopen_variation14.tmp ---
test passed
--- READ: file:///%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
--- WRITE: file:///%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
--- READ: file://../fopen14.tmpDir/fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://../fopen14.tmpDir/fopen_variation14.tmp in %s on line %d

Warning: fopen(file://../fopen14.tmpDir/fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- WRITE: file://../fopen14.tmpDir/fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://../fopen14.tmpDir/fopen_variation14.tmp in %s on line %d

Warning: fopen(file://../fopen14.tmpDir/fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- READ: file://..\fopen14.tmpDir\fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://..\fopen14.tmpDir\fopen_variation14.tmp in %s on line %d

Warning: fopen(file://..\fopen14.tmpDir\fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- WRITE: file://..\fopen14.tmpDir\fopen_variation14.tmp ---

Warning: fopen(): Remote host file access not supported, file://..\fopen14.tmpDir\fopen_variation14.tmp in %s on line %d

Warning: fopen(file://..\fopen14.tmpDir\fopen_variation14.tmp): Failed to open stream: no suitable wrapper could be found in %s on line %d
--- READ: file://%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
--- WRITE: file://%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
--- READ: file://%s/fopen14.tmpDir\fopen_variation14.tmp ---
test passed
--- WRITE: file://%s/fopen14.tmpDir\fopen_variation14.tmp ---
test passed
--- READ: file:///%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
--- WRITE: file:///%s/fopen14.tmpDir/fopen_variation14.tmp ---
test passed
