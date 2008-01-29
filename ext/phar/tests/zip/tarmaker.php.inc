<?php
// stolen from PEAR2_Pyrus_Developer_Creator_Zip by Greg Beaver, the original author, for use in unit tests
class tarmaker
{
    /**
     * Path to archive file
     *
     * @var string
     */
    protected $archive;
    /**
     * @var Phar
     */
    protected $zip;
    protected $path;
    /**
     * this is the location we'll create the phar, then we'll copy() it to $path
     * @var string
     */
    protected $tmppath;
    function __construct($path)
    {
        $this->path = $path;
        $this->tmppath = dirname($path) . "/.tmp.thingy.phar";
    }

    /**
     * save a file inside this package
     * @param string relative path within the package
     * @param string|resource file contents or open file handle
     */
    function addFile($path, $fileOrStream)
    {
        if ($path == '.phar/stub.php') {
            $this->zip->setStub($fileOrStream);
        } elseif ($path == '.phar/alias.txt') {
            $this->zip->setAlias($fileOrStream);
        }
        $this->zip[$path] = $fileOrStream;
    }

    /**
     * Initialize the package creator
     */
    function init()
    {
        $this->zip = new Phar($this->tmppath);
        $this->zip->convertToZip();
    }

    /**
     * Create an internal directory, creating parent directories as needed
     * 
     * This is a no-op for the tar creator
     * @param string $dir
     */
    function mkdir($dir)
    {
        if ($dir[strlen($dir)-1] != '/') $dir .= '/';
        $this->zip[$dir] = '';
    }

    /**
     * Finish saving the package
     */
    function close()
    {
        $this->zip->stopBuffering();
        copy($this->tmppath, $this->path);
        $this->zip->setAlias('not.to.be.used'); // for preventing alias conflict
        $this->zip->stopBuffering();
    }

    function __destruct()
    {
        @unlink($this->tmppath);
    }
}